"""
MkDocs hook: point the markmap plugin at the JS bundles vendored in
docs/javascripts/ instead of unpkg.com (unreachable behind the corporate proxy).

MkDocs already copies docs/javascripts/* into site/javascripts/*, so the only
thing left to do is rewrite the CDN <script src> emitted by mkdocs-markmap.

Registered in mkdocs.yml under `hooks:`.
"""

import re

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
        pattern = r'(<script[^>]*src=")[^"]*' + re.escape(cdn_fragment) + r'[^"]*(")'
        output = re.sub(pattern, rf"\g<1>/javascripts/{filename}\g<2>", output)

    return output
