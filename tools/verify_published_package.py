import argparse
import hashlib
import json
import pathlib
import re
import sys
import time
import urllib.error
import urllib.request
import zipfile


def publication_request(url):
    return urllib.request.Request(url, headers={
        'Accept': 'application/json, application/zip',
        'User-Agent': 'GimmiThemLegsPublisher/1.0',
    })


def verify_publication(package_path, namespace):
    with zipfile.ZipFile(package_path) as package:
        manifest = json.loads(package.read('manifest.json'))
    package_name = manifest['name']
    package_version = manifest['version_number']
    if not all(re.fullmatch(r'[A-Za-z0-9_]+', value) for value in (namespace, package_name)):
        raise ValueError('GimmiThemLegsInvalidPublicationIdentity')
    if not re.fullmatch(r'\d+\.\d+\.\d+', package_version):
        raise ValueError('GimmiThemLegsInvalidPublicationVersion')
    version_url = f'https://thunderstore.io/api/experimental/package/{namespace}/{package_name}/{package_version}/?publication_check={time.time_ns()}'
    try:
        with urllib.request.urlopen(publication_request(version_url), timeout=30) as response:
            publication = json.load(response)
    except urllib.error.HTTPError as error:
        if error.code == 404:
            return None
        raise
    for field in ('name', 'version_number', 'description', 'website_url', 'dependencies'):
        if publication.get(field) != manifest[field]:
            raise ValueError('GimmiThemLegsPublishedMetadataMismatch_' + field)
    if publication.get('namespace') != namespace or publication.get('is_active') is not True:
        raise ValueError('GimmiThemLegsPublishedIdentityMismatch')
    download_url = f'https://thunderstore.io/package/download/{namespace}/{package_name}/{package_version}/'
    if publication.get('download_url') != download_url:
        raise ValueError('GimmiThemLegsPublishedDownloadMismatch')
    with package_path.open('rb') as package_file:
        expected_digest = hashlib.file_digest(package_file, 'sha256').hexdigest()
    published_digest = hashlib.sha256()
    published_bytes = 0
    with urllib.request.urlopen(publication_request(download_url), timeout=30) as response:
        while chunk := response.read(1024 * 1024):
            published_bytes += len(chunk)
            if published_bytes > package_path.stat().st_size:
                raise ValueError('GimmiThemLegsPublishedPackageSizeMismatch')
            published_digest.update(chunk)
    if published_digest.hexdigest() != expected_digest:
        raise ValueError('GimmiThemLegsPublishedPackageContentMismatch')
    return dict(version=package_version, sha256=expected_digest, bytes=published_bytes, published=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('package', type=pathlib.Path)
    parser.add_argument('--namespace', required=True)
    parser.add_argument('--wait', type=int, default=0)
    options = parser.parse_args()
    deadline = time.monotonic() + max(0, options.wait)
    while True:
        try:
            publication = verify_publication(options.package, options.namespace)
            if publication:
                print(json.dumps(publication))
                sys.exit(0)
            if time.monotonic() >= deadline:
                print(json.dumps(dict(published=False)))
                sys.exit(3)
        except (OSError, ValueError) as error:
            if time.monotonic() >= deadline or isinstance(error, ValueError):
                print(json.dumps(dict(error=str(error))))
                sys.exit(1)
        time.sleep(min(10, max(0, deadline - time.monotonic())))
