import { useEffect, useMemo, useRef } from 'react'
import cytoscape, { type Core, type ElementDefinition } from 'cytoscape'
import dagre from 'cytoscape-dagre'
import { useLanguage } from './i18n'
import type { GraphData, GraphNode } from './types'

cytoscape.use(dagre)

type Scope = 'main' | 'all' | 'uncovered'

type Props = {
  graph: GraphData
  scope: Scope
  recursionDepth: 'normal' | number
  search: string
  onSelect: (node: GraphNode) => void
  onVisibleCount: (count: number) => void
}

/** Above this size a force-free grid layout is the only interactive option. */
const HIERARCHICAL_LAYOUT_LIMIT = 160

const styles = [
  { selector: 'node', style: {
    'background-color': '#173d29', 'border-color': '#47c95e', 'border-width': 1.4,
    color: '#f0f6fc', label: 'data(short)', width: 'label', height: 28,
    padding: '12px', shape: 'roundrectangle', 'font-size': 11, 'text-valign': 'center',
    'text-max-width': 180, 'text-wrap': 'ellipsis', 'overlay-opacity': 0,
  } },
  { selector: 'node[status="partial"]', style: { 'background-color': '#453915', 'border-color': '#e3ad22', color: '#fff5cc' } },
  { selector: 'node[status="uncovered"]', style: { 'background-color': '#262d37', 'border-color': '#677180', color: '#c2cad4' } },
  { selector: 'node[comparison="common"]', style: { 'background-color': '#173d29', 'border-color': '#47c95e', color: '#f0fff3' } },
  { selector: 'node[comparison="left_only"]', style: { 'background-color': '#123b64', 'border-color': '#4ca5ff', color: '#edf7ff' } },
  { selector: 'node[comparison="right_only"]', style: { 'background-color': '#543d0e', 'border-color': '#f0b72f', color: '#fff8df' } },
  { selector: 'node[comparison="neither"]', style: { 'background-color': '#262d37', 'border-color': '#677180', color: '#aeb8c3' } },
  { selector: 'node.hidden', style: { display: 'none' } },
  { selector: 'node:selected', style: { 'border-color': '#4ca5ff', 'border-width': 3, 'background-color': '#123b64' } },
  { selector: 'edge', style: {
    width: 1.15, 'line-color': '#596472', 'target-arrow-color': '#596472',
    'target-arrow-shape': 'triangle', 'curve-style': 'bezier', 'arrow-scale': .75,
    opacity: .7, 'overlay-opacity': 0,
  } },
  { selector: 'edge[covered]', style: { 'line-color': '#37a954', 'target-arrow-color': '#37a954', opacity: .86 } },
  { selector: 'edge.hidden', style: { display: 'none' } },
]

function visibleIds(graph: GraphData, scope: Scope, recursionDepth: 'normal' | number): Set<string> {
  return new Set(graph.nodes.filter(node => {
    if (recursionDepth !== 'normal' && (node.recursion_depth === null || node.recursion_depth > recursionDepth)) return false
    if (scope === 'main') return node.status === 'covered' || node.status === 'partial'
    if (scope === 'uncovered') return node.status === 'partial' || node.status === 'uncovered'
    return true
  }).map(node => node.id))
}

export default function GraphCanvas({ graph, scope, recursionDepth, search, onSelect, onVisibleCount }: Props) {
  const { t } = useLanguage()
  const container = useRef<HTMLDivElement>(null)
  const instance = useRef<Core | null>(null)
  const selectHandler = useRef(onSelect)
  const ids = useMemo(() => visibleIds(graph, scope, recursionDepth), [graph, scope, recursionDepth])

  // Keep the tap handler current without tearing down the cytoscape instance.
  useEffect(() => { selectHandler.current = onSelect }, [onSelect])

  // Build the instance once per graph payload. Filtering afterwards only toggles
  // classes, so changing scope or recursion depth never pays for a full rebuild.
  useEffect(() => {
    if (!container.current) return
    const nodes: ElementDefinition[] = graph.nodes.map(node => ({ data: node }))
    const edges: ElementDefinition[] = graph.edges.map((edge, index) => ({ data: { id: `e${index}`, ...edge } }))
    const cy = cytoscape({
      container: container.current,
      elements: [...nodes, ...edges],
      style: styles as never,
      minZoom: .08,
      maxZoom: 3.5,
      wheelSensitivity: .18,
      textureOnViewport: nodes.length > HIERARCHICAL_LAYOUT_LIMIT,
      hideEdgesOnViewport: nodes.length > HIERARCHICAL_LAYOUT_LIMIT,
      pixelRatio: 1,
    })
    instance.current = cy
    cy.on('tap', 'node', event => selectHandler.current(event.target.data() as GraphNode))
    return () => { cy.destroy(); instance.current = null }
  }, [graph])

  // Apply the current filter and re-run layout only over the visible subset.
  useEffect(() => {
    const cy = instance.current
    if (!cy) return
    onVisibleCount(ids.size)
    cy.batch(() => {
      cy.nodes().forEach(node => {
        node.toggleClass('hidden', !ids.has(node.id()))
      })
      cy.edges().forEach(edge => {
        const data = edge.data()
        edge.toggleClass('hidden', !ids.has(data.source) || !ids.has(data.target))
      })
    })
    const visible = cy.nodes().not('.hidden')
    if (!visible.length) return
    const hierarchical = recursionDepth !== 'normal' || visible.length <= HIERARCHICAL_LAYOUT_LIMIT
    const layout = hierarchical
      ? { name: 'dagre', rankDir: 'TB', nodeSep: 34, rankSep: 48, edgeSep: 12, padding: 42, animate: false }
      : { name: 'grid', cols: Math.ceil(Math.sqrt(visible.length * 1.7)), condense: true, avoidOverlap: true, spacingFactor: 1.15, padding: 30 }
    visible.union(visible.edgesWith(visible)).layout(layout as never).run()
    cy.fit(visible, 35)
  }, [ids, onVisibleCount, recursionDepth])

  useEffect(() => {
    const term = search.trim().toLowerCase()
    const cy = instance.current
    if (!term || !cy) return
    const match = cy.nodes().not('.hidden').filter(node => String(node.data('label')).toLowerCase().includes(term)).first()
    if (match.length) {
      cy.nodes().unselect()
      match.select()
      cy.animate({ center: { eles: match }, zoom: Math.max(cy.zoom(), .9) }, { duration: 240 })
      selectHandler.current(match.data() as GraphNode)
    }
  }, [search])

  const zoomBy = (factor: number) => {
    const cy = instance.current
    if (cy) cy.zoom({ level: cy.zoom() * factor, renderedPosition: { x: cy.width() / 2, y: cy.height() / 2 } })
  }

  return <div className="graph-wrap">
    <div ref={container} className="graph-canvas" />
    <div className="graph-tools" aria-label={t('graph.controls')}>
      <button title={t('graph.fit')} onClick={() => instance.current?.fit(instance.current.nodes().not('.hidden'), 35)}>⌗</button>
      <button title={t('graph.zoomIn')} onClick={() => zoomBy(1.25)}>＋</button>
      <button title={t('graph.zoomOut')} onClick={() => zoomBy(1 / 1.25)}>−</button>
      <button title={t('graph.center')} onClick={() => instance.current?.center(instance.current.nodes().not('.hidden'))}>◎</button>
    </div>
  </div>
}
