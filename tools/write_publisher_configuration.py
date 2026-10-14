import argparse
import json
import pathlib
import re
import zipfile


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('package')
    parser.add_argument('--namespace', required=True)
    parser.add_argument('--output', required=True)
    options = parser.parse_args()
    if not re.fullmatch(r'[A-Za-z0-9_]+', options.namespace):
        raise ValueError('GimmiThemLegsInvalidPublisherNamespace')
    root = pathlib.Path(__file__).resolve().parent.parent
    with zipfile.ZipFile(options.package) as package:
        manifest = json.loads(package.read('manifest.json'))
    if manifest['name'] != 'GimmiThemLegs_pre_alpha':
        raise ValueError('GimmiThemLegsPublisherPackageMismatch')
    configuration = (root / 'thunderstore.toml').read_text(encoding='utf-8')
    configuration = re.sub(r'^namespace = "[^"]*"$', 'namespace = ' + json.dumps(options.namespace), configuration, flags=re.MULTILINE)
    configuration = re.sub(r'^versionNumber = "[^"]*"$', 'versionNumber = ' + json.dumps(manifest['version_number']), configuration, flags=re.MULTILINE)
    pathlib.Path(options.output).write_text(configuration, encoding='utf-8')
