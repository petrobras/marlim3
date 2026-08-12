/**
 * markmap-fullsize.js
 *
 * Makes every .mkdocs-markmap container fill the available viewport height and
 * re-fits the diagram on window resize.
 *
 * The mkdocs-markmap plugin sizes the container proportionally to the diagram
 * inside a requestAnimationFrame callback, so heights are re-applied on a short
 * timeout to land after it.
 */

(function () {
  "use strict";

  const CONTAINERS = ".mkdocs-markmap";

  // markmap-view exposes no instance registry, so capture instances as the
  // plugin creates them. This file is loaded before the plugin's inline
  // initializeMarkmap() script, so the patch is always in place in time.
  const create = window.markmap?.Markmap?.create;
  if (create) {
    window.markmap.Markmap.create = function (svg, ...rest) {
      const instance = create.call(this, svg, ...rest);
      const node = svg && svg.node ? svg.node() : svg;
      if (node) node.__markmapInstance = instance;
      return instance;
    };
  }

  function targetHeight() {
    const header = document.querySelector(".md-header");
    const headerH = header ? header.getBoundingClientRect().height : 48;
    return Math.max(window.innerHeight - headerH - 16, 300);
  }

  // Resize the container, then tell the markmap instance to re-fit its content.
  function resize(el) {
    el.style.height = targetHeight() + "px";
    const instance = el.querySelector("svg.markmap")?.__markmapInstance;
    if (instance) instance.fit();
  }

  function watch(el) {
    if (el._fullsizeWatched) return;
    el._fullsizeWatched = true;

    resize(el); // avoid a flash at the plugin's default height

    // The SVG is injected asynchronously once markmap-lib has loaded.
    const observer = new MutationObserver(() => {
      if (!el.querySelector("svg.markmap")) return;
      observer.disconnect();
      setTimeout(() => resize(el), 120);
    });
    observer.observe(el, { childList: true });
  }

  function init() {
    document.querySelectorAll(CONTAINERS).forEach(watch);
  }

  window.addEventListener("resize", () => {
    document.querySelectorAll(CONTAINERS).forEach((el) => {
      el.style.height = targetHeight() + "px";
      setTimeout(() => resize(el), 60); // let layout settle before fit()
    });
  });

  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", init);
  } else {
    init();
  }

  // MkDocs Material instant navigation swaps the page content in place.
  if (typeof document$ !== "undefined") document$.subscribe(init);
})();
