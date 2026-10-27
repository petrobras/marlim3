"""
MkDocs hook: point the markmap plugin at the JS bundles vendored in
docs/javascripts/ instead of unpkg.com (unreachable behind the corporate proxy).

MkDocs already copies docs/javascripts/* into site/javascripts/*, so the only
thing left to do is rewrite the CDN <script src> emitted by mkdocs-markmap.

The rewritten path must be relative to the current page, not root-absolute:
sites deployed under a subpath (e.g. Read the Docs' /en/latest/) break if we
hardcode "/javascripts/...", since that resolves against the domain root
instead of the site's actual base path.

Registered in mkdocs.yml under `hooks:`.
"""

import re

from mkdocs.utils import get_relative_url

# CDN URL fragment -> vendored file in docs/javascripts/
JS_MAP = {
    "d3@7": "d3.v7.min.js",
    "markmap-lib@0.18": "markmap-lib.0.18.js",
    "markmap-view@0.18": "markmap-view.0.18.js",
}


def on_post_page(output, page, config, **kwargs):
    if "mkdocs-markmap" not in output:
        return output

    for cdn_fragment, filename in JS_MAP.items():
        rel_url = get_relative_url(f"javascripts/{filename}", page.url)
        pattern = r'(<script[^>]*src=")[^"]*' + re.escape(cdn_fragment) + r'[^"]*(")'
        output = re.sub(pattern, rf"\g<1>{rel_url}\g<2>", output)

    return output
