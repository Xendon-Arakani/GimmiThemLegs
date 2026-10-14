import argparse
import json
import os
import pathlib
import re
import struct
import zipfile


def synchronize_release_version(root, version):
    manifest_path = root / 'manifest.json'
    manifest = json.loads(manifest_path.read_text(encoding='utf-8'))
    version = version or manifest['version_number']
    if not re.fullmatch(r'(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)', version):
        raise ValueError('GimmiThemLegsInvalidReleaseVersion')
    manifest['version_number'] = version
    manifest_path.write_text(json.dumps(manifest, indent=2) + '\n', encoding='utf-8')
    mod_path = root / 'mods' / 'GimmiThemLegs' / 'mod.json'
    mod = json.loads(mod_path.read_text(encoding='utf-8'))
    mod['Version'] = version
    for convar in mod['ConVars']:
        if convar['Name'] == 'first_person_body_version':
            convar['DefaultValue'] = version
    mod_path.write_text(json.dumps(mod, indent=2) + '\n', encoding='utf-8')
    publisher_configuration_path = root / 'thunderstore.toml'
    publisher_configuration = publisher_configuration_path.read_text(encoding='utf-8')
    publisher_configuration = re.sub(r'^versionNumber = "[^"]*"$', 'versionNumber = ' + json.dumps(version), publisher_configuration, flags=re.MULTILINE)
    publisher_configuration_path.write_text(publisher_configuration, encoding='utf-8')
    return manifest, mod


def build_release_package(root, manifest, mod):
    description = 'Making legs great again.'
    if manifest['description'] != description or mod['Description'] != description or (root / 'README.md').read_text(encoding='utf-8') != description:
        raise ValueError('GimmiThemLegsDescriptionMismatch')
    if mod['Name'] != 'GimmiThemLegs [pre-alpha]' or manifest['name'] != 'GimmiThemLegs_pre_alpha':
        raise ValueError('GimmiThemLegsNameMismatch')
    if any(script['RunOn'] != 'CLIENT && MP' or 'ServerCallback' in script for script in mod['Scripts']):
        raise ValueError('GimmiThemLegsClientOnlyScriptsRequired')
    if any(command['Context'] != 'CLIENT' for command in mod['ConCommands']):
        raise ValueError('GimmiThemLegsClientOnlyCommandsRequired')
    icon = (root / 'icon.png').read_bytes()
    if icon[:8] != b'\x89PNG\r\n\x1a\n' or struct.unpack_from('>II', icon, 16) != (256, 256):
        raise ValueError('GimmiThemLegsIconInvalid')
    models = list((root / 'mods' / 'GimmiThemLegs' / 'mod' / 'models').rglob('*.mdl'))
    if len(models) != 238:
        raise ValueError('GimmiThemLegsPilotModelsMissing')
    plugin_path = root / 'plugins' / 'FirstPersonBodyBones.dll'
    plugin = plugin_path.read_bytes()
    if (mod['Name'] + ' ' + manifest['version_number']).encode() not in plugin:
        raise ValueError('GimmiThemLegsPluginVersionMismatch')
    output = root / 'dist' / (manifest['name'] + '-' + manifest['version_number'] + '.zip')
    output.parent.mkdir(exist_ok=True)
    files = [root / 'manifest.json', root / 'README.md', root / 'icon.png', plugin_path]
    files.extend(sorted(path for path in (root / 'mods' / 'GimmiThemLegs').rglob('*') if path.is_file()))
    with zipfile.ZipFile(output, 'w', zipfile.ZIP_DEFLATED, compresslevel=6) as archive:
        for path in files:
            archive.write(path, path.relative_to(root).as_posix())
    with zipfile.ZipFile(output) as archive:
        if archive.testzip() is not None:
            raise ValueError('GimmiThemLegsPackageIntegrityFailed')
    if os.environ.get('GITHUB_OUTPUT'):
        with open(os.environ['GITHUB_OUTPUT'], 'a', encoding='utf-8') as output_file:
            output_file.write('version=' + manifest['version_number'] + '\n')
            output_file.write('package=' + output.relative_to(root).as_posix() + '\n')
    print(json.dumps(dict(package=str(output), version=manifest['version_number'], bytes=output.stat().st_size)))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--version')
    parser.add_argument('--synchronize-only', action='store_true')
    options = parser.parse_args()
    root = pathlib.Path(__file__).resolve().parent.parent
    manifest, mod = synchronize_release_version(root, options.version)
    if not options.synchronize_only:
        build_release_package(root, manifest, mod)
