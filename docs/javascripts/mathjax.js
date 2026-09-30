window.MathJax = {
  tex: {
    // "$"/"$$" needed for notebook cells (mkdocs-jupyter/nbconvert output bypasses pymdownx.arithmatex)
    inlineMath: [["\\(", "\\)"], ["$", "$"]],
    displayMath: [["\\[", "\\]"], ["$$", "$$"]],
    processEscapes: true,
    tags: "all"
  }
};

document$.subscribe(() => {
  MathJax.texReset();
  MathJax.typesetClear();
  MathJax.typesetPromise();
});