import { memo, useCallback, useDeferredValue, useEffect, useMemo, useRef, useState } from 'react'
import {
  Braces, Check, ChevronDown, ChevronUp, Code2, FileText, GitCompare, Hammer, Info,
  MoreVertical, Play, Plus, Search, Settings2, SlidersHorizontal, Terminal, UploadCloud, X,
} from 'lucide-react'
import { api } from './api'
import GraphCanvas from './GraphCanvas'
import { LanguageProvider, useLanguage, type Language, type Translate } from './i18n'
import type {
  Bootstrap, CoverageMetric, CoverageSummary, GraphData, GraphNode, Job, JobItem, ModelFile,
  RegressionTest, SimulationLog, SourceData,
} from './types'

type Scope = 'main' | 'all' | 'uncovered'
type BottomTab = 'code' | 'execution' | 'validation' | 'terminal' | 'logs'
type RecursionDepth = 'normal' | number

const emptyMetric: CoverageMetric = { covered: 0, total: 0, percent: 0 }
const MODEL_PATTERN = /\.(mr3|json)$/i

function useNumberFormat(locale: string) {
  const formatter = useMemo(() => new Intl.NumberFormat(locale), [locale])
  return useCallback((value: number) => formatter.format(value), [formatter])
}

function LanguageToggle() {
  const { language, setLanguage, t } = useLanguage()
  const options: Language[] = ['en', 'pt']
  return <div className="language-toggle" role="group" aria-label={t('language.label')}>
    {options.map(option => <button
      key={option}
      type="button"
      className={language === option ? 'active' : ''}
      aria-pressed={language === option}
      title={t(option === 'en' ? 'language.en' : 'language.pt')}
      onClick={() => setLanguage(option)}
    >{option.toUpperCase()}</button>)}
  </div>
}

function CoverageCard({ title, metric, tone, ratio = false, format }: {
  title: string; metric: CoverageMetric; tone: string; ratio?: boolean; format: (value: number) => string
}) {
  return <div className="coverage-card">
    <span>{title}</span>
    <strong style={{ color: tone }}>{ratio ? `${format(metric.covered)}/${format(metric.total)}` : `${metric.percent.toFixed(0)}%`}</strong>
    <div className="meter"><i style={{ width: `${Math.min(100, metric.percent)}%`, background: tone }} /></div>
  </div>
}

function ExecutionStatusIcon({ executed, running, error, status, t }: {
  executed: boolean; running: boolean; error: boolean; status: string; t: Translate
}) {
  if (running) return <span className="test-status running" title={t('tests.statusRunning')} />
  if (error || (executed && status !== 'passed')) {
    return <span className="test-status failed" title={t('tests.statusFailed', { status })}><X size={12} strokeWidth={2.8} /></span>
  }
  if (executed) return <span className="test-status passed" title={t('tests.statusPassed')}><Check size={14} strokeWidth={2.7} /></span>
  return <span className="test-status pending" title={t('tests.statusPending')} />
}

function ResultStatusIcon({ status }: { status: string }) {
  if (status === 'passed') return <span className="test-status passed"><Check size={15} /></span>
  if (status === 'pending') return <span className="test-status pending" />
  return <span className="test-status failed"><X size={12} strokeWidth={2.8} /></span>
}

/**
 * A single test row. Memoised so toggling one selection does not re-render the
 * whole list, which is what made multi-selection feel sluggish.
 */
const TestRow = memo(function TestRow({ test, selected, live, onToggle, t }: {
  test: RegressionTest
  selected: boolean
  live?: JobItem
  onToggle: (test: RegressionTest) => void
  t: Translate
}) {
  return <button className={`test-row ${selected ? 'selected' : ''}`} onClick={() => onToggle(test)}>
    <span className={`check-box ${selected ? 'checked' : ''}`}>{selected && <Check size={14} />}</span>
    <span className="test-name">{test.name}</span>
    <ExecutionStatusIcon
      executed={test.latest.has_run || Boolean(live?.has_run)}
      running={live?.state === 'running'}
      error={live?.state === 'error'}
      status={live?.result_status || test.latest.status}
      t={t}
    />
  </button>
})

function fileToBase64(file: File): Promise<string> {
  return new Promise((resolve, reject) => {
    const reader = new FileReader()
    reader.onerror = () => reject(new Error(`Could not read ${file.name}`))
    reader.onload = () => {
      const result = String(reader.result || '')
      resolve(result.includes(',') ? result.slice(result.indexOf(',') + 1) : result)
    }
    reader.readAsDataURL(file)
  })
}

/**
 * Registers one or many models at once. Names are never typed: each test is
 * named after its own input file, so the dialog only collects files and a group.
 */
function AddTestDialog({ onClose, onSaved }: { onClose: () => void; onSaved: () => void }) {
  const { t } = useLanguage()
  const [files, setFiles] = useState<File[]>([])
  const [suite, setSuite] = useState('custom')
  const [error, setError] = useState('')
  const [saving, setSaving] = useState(false)

  const addFiles = (incoming: FileList | null) => {
    const accepted = Array.from(incoming || []).filter(file => MODEL_PATTERN.test(file.name))
    if (!accepted.length) {
      setError(t('add.error'))
      return
    }
    setError('')
    // Re-selecting the same file replaces it instead of creating a duplicate test.
    setFiles(current => [...current.filter(file => !accepted.some(next => next.name === file.name)), ...accepted])
  }

  const save = async () => {
    if (!files.length) {
      setError(t('add.error'))
      return
    }
    setSaving(true)
    try {
      const models: ModelFile[] = await Promise.all(
        files.map(async file => ({ name: file.name, content_base64: await fileToBase64(file) })),
      )
      await api.saveTestsBatch(models, suite.trim() || 'custom')
      onSaved()
      onClose()
    } catch (reason) {
      setError(reason instanceof Error ? reason.message : String(reason))
    } finally {
      setSaving(false)
    }
  }

  const summary = files.length === 1 ? t('add.selectedOne') : t('add.selectedMany', { count: files.length })

  return <div className="modal-backdrop" onMouseDown={event => { if (event.currentTarget === event.target) onClose() }}>
    <div className="modal">
      <h2>{t('add.title')}</h2>
      <label
        className="file-drop"
        onDragOver={event => event.preventDefault()}
        onDrop={event => { event.preventDefault(); addFiles(event.dataTransfer.files) }}
      >
        <UploadCloud size={28} />
        <strong>{files.length ? summary : t('add.dropHere')}</strong>
        <span>{files.length ? t('add.chooseOther') : t('add.dropHint')}</span>
        <input type="file" accept=".mr3,.json" multiple onChange={event => addFiles(event.target.files)} />
      </label>
      {files.length > 0 && <ul className="model-list">
        {files.map(file => <li key={file.name}>
          <span title={file.name}>{file.name.replace(MODEL_PATTERN, '')}</span>
          <button type="button" title={t('add.remove')} onClick={() => setFiles(current => current.filter(item => item !== file))}>
            <X size={13} />
          </button>
        </li>)}
      </ul>}
      <p className="field-hint">{t('add.nameHint')}</p>
      <label>{t('add.group')}<input value={suite} onChange={event => setSuite(event.target.value)} /></label>
      {error && <p className="error-text">{error}</p>}
      <div className="modal-actions">
        <button onClick={onClose} disabled={saving}>{t('add.cancel')}</button>
        <button className="primary" onClick={() => void save()} disabled={saving || !files.length}>
          {saving ? t('add.saving') : t('add.save')}
        </button>
      </div>
    </div>
  </div>
}

function ComparisonCoverage({ graph }: { graph: GraphData }) {
  const { t } = useLanguage()
  const comparison = graph.comparison
  if (!comparison) return null
  const rows: { key: keyof CoverageSummary; label: string }[] = [
    { key: 'lines', label: t('coverage.lines') }, { key: 'functions', label: t('coverage.functions') },
    { key: 'branches', label: t('coverage.branches') }, { key: 'files', label: t('coverage.files') },
  ]
  const delta = comparison.delta.line_percent_delta
  return <div className="comparison-coverage">
    <div className="comparison-heading"><span>{comparison.left.name}</span><span>{comparison.right.name}</span></div>
    {rows.map(row => <div className="comparison-metric" key={row.key}>
      <span>{row.label}</span>
      <b>{comparison.left.summary[row.key].percent.toFixed(1)}%</b>
      <b>{comparison.right.summary[row.key].percent.toFixed(1)}%</b>
    </div>)}
    <div className={`comparison-delta ${delta >= 0 ? 'positive' : 'negative'}`}>
      {t('comparison.lineDelta', { value: `${delta >= 0 ? '+' : ''}${delta.toFixed(2)}` })}
    </div>
  </div>
}

function RegressionApp() {
  const { t, locale } = useLanguage()
  const compact = useNumberFormat(locale)
  const [bootstrap, setBootstrap] = useState<Bootstrap | null>(null)
  const [graph, setGraph] = useState<GraphData | null>(null)
  const [selectedTests, setSelectedTests] = useState<string[]>([])
  const [selectedNode, setSelectedNode] = useState<GraphNode | null>(null)
  const [source, setSource] = useState<SourceData | null>(null)
  const [testSearch, setTestSearch] = useState('')
  const [functionInput, setFunctionInput] = useState('')
  const [functionSearch, setFunctionSearch] = useState('')
  const [scope, setScope] = useState<Scope>('all')
  const [recursionDepth, setRecursionDepth] = useState<RecursionDepth>('normal')
  const [comparisonMode, setComparisonMode] = useState(false)
  const [bottomTab, setBottomTab] = useState<BottomTab>('code')
  const [visibleCount, setVisibleCount] = useState(0)
  const [loading, setLoading] = useState(true)
  const [graphLoading, setGraphLoading] = useState(false)
  const [error, setError] = useState('')
  const [job, setJob] = useState<Job | null>(null)
  const [simulationLog, setSimulationLog] = useState<SimulationLog | null>(null)
  const [simulationLogLoading, setSimulationLogLoading] = useState(false)
  const [coverageOpen, setCoverageOpen] = useState(true)
  const [contributionOpen, setContributionOpen] = useState(true)
  const [detailsOpen, setDetailsOpen] = useState(true)
  const [showAdd, setShowAdd] = useState(false)
  const terminalRef = useRef<HTMLDivElement>(null)

  const formatTime = useCallback((value?: string) => {
    if (!value) return t('status.notRun')
    return new Intl.DateTimeFormat(locale, { dateStyle: 'short', timeStyle: 'medium' }).format(new Date(value))
  }, [locale, t])

  const refresh = useCallback(async () => {
    try {
      const data = await api.bootstrap()
      setBootstrap(data)
      setSelectedTests(previous => previous.filter(id => data.tests.some(test => test.id === id)))
      setError('')
    } catch (reason) {
      setError(reason instanceof Error ? reason.message : String(reason))
    } finally {
      setLoading(false)
    }
  }, [])

  useEffect(() => { void refresh() }, [refresh])

  // Keyed by a stable, order-independent string so re-selecting the same set
  // never refetches the graph.
  const selectionKey = useMemo(() => [...selectedTests].sort().join(','), [selectedTests])
  const testsById = useMemo(
    () => new Map((bootstrap?.tests || []).map(test => [test.id, test])),
    [bootstrap],
  )
  const selectedTestData = useMemo(
    () => selectedTests.map(id => testsById.get(id)).filter((test): test is RegressionTest => Boolean(test)),
    [selectedTests, testsById],
  )
  const analysisAllowed = selectedTestData.length > 0
    && selectedTestData.length === selectedTests.length
    && selectedTestData.every(test => test.latest.has_coverage)
  const canCompare = analysisAllowed && selectedTests.length === 2
  const liveRunItems = useMemo(
    () => new Map((job?.kind === 'run' ? job.items || [] : []).map(item => [item.test_id, item])),
    [job],
  )

  useEffect(() => {
    if (!bootstrap || !analysisAllowed) {
      setGraph(null)
      setSelectedNode(null)
      setSource(null)
      setVisibleCount(0)
      setGraphLoading(false)
      return
    }
    let cancelled = false
    setGraph(null)
    setSelectedNode(null)
    setSource(null)
    setVisibleCount(0)
    setGraphLoading(true)
    const ids = selectionKey ? selectionKey.split(',') : []
    const request = comparisonMode ? api.compare(ids) : api.graph(ids)
    request.then(data => {
      if (!cancelled) {
        setGraph(data)
        setError('')
      }
    }).catch(reason => {
      if (!cancelled) setError(reason instanceof Error ? reason.message : String(reason))
    }).finally(() => {
      if (!cancelled) setGraphLoading(false)
    })
    return () => { cancelled = true }
  }, [analysisAllowed, bootstrap, comparisonMode, selectionKey])

  useEffect(() => {
    if (recursionDepth !== 'normal' && graph && recursionDepth > graph.max_recursion_depth) setRecursionDepth('normal')
  }, [graph, recursionDepth])

  useEffect(() => {
    if (!job || job.status !== 'running') return
    let active = true
    let timer = 0
    const poll = async () => {
      try {
        const next = await api.job(job.id)
        if (!active) return
        setJob(next)
        if (next.status !== 'running') {
          await refresh()
          return
        }
      } catch (reason) {
        if (active) setError(reason instanceof Error ? reason.message : String(reason))
      }
      // Chained timeouts, so a slow response can never queue overlapping polls.
      if (active) timer = window.setTimeout(() => void poll(), 600)
    }
    timer = window.setTimeout(() => void poll(), 600)
    return () => { active = false; window.clearTimeout(timer) }
  }, [job, refresh])

  useEffect(() => {
    if (bottomTab !== 'terminal' || !terminalRef.current) return
    terminalRef.current.scrollTop = terminalRef.current.scrollHeight
  }, [bottomTab, job?.terminal?.length])

  const selectNode = useCallback((node: GraphNode) => {
    if (!analysisAllowed) return
    setSelectedNode(node)
    setBottomTab('code')
    api.source(node.file, node.line, node.end_line, selectedTests).then(setSource)
      .catch(reason => setError(reason instanceof Error ? reason.message : String(reason)))
  }, [analysisAllowed, selectedTests])
  const updateVisible = useCallback((count: number) => setVisibleCount(count), [])

  useEffect(() => {
    if (!graph) return
    const initial = graph.nodes.find(node => node.short === 'main') || graph.nodes[0]
    if (initial && (!selectedNode || !graph.nodes.some(node => node.id === selectedNode.id))) selectNode(initial)
  }, [graph, selectNode, selectedNode])

  // Deferring the search term keeps typing responsive on long test lists.
  const deferredSearch = useDeferredValue(testSearch)
  const filteredTests = useMemo(() => {
    const term = deferredSearch.toLowerCase().trim()
    if (!term) return bootstrap?.tests || []
    return (bootstrap?.tests || []).filter(test =>
      `${test.name} ${test.suite} ${test.model_path}`.toLowerCase().includes(term))
  }, [bootstrap, deferredSearch])
  const suites = useMemo(() => [...new Set((bootstrap?.tests || []).map(test => test.suite))], [bootstrap])
  const summary = graph?.summary
  const { passed, failed, lastRun } = useMemo(() => {
    const tests = bootstrap?.tests || []
    return {
      passed: tests.filter(test => test.latest.status === 'passed').length,
      failed: tests.filter(test => test.latest.has_run && test.latest.status !== 'passed').length,
      lastRun: tests.map(test => test.latest.started_at).filter(Boolean).sort().at(-1),
    }
  }, [bootstrap])
  const workflow = bootstrap?.workflow || { validated: false, compiled: false, ready: false }

  const toggleTest = useCallback((test: RegressionTest) => {
    setComparisonMode(false)
    setSelectedNode(null)
    setSource(null)
    setSelectedTests(ids => ids.includes(test.id) ? ids.filter(id => id !== test.id) : [...ids, test.id])
  }, [])

  const runAction = async (kind: 'build' | 'run') => {
    try {
      const next = kind === 'build' ? await api.build() : await api.run(selectedTests)
      if (kind === 'run') setSimulationLog(null)
      setJob(next)
      setBottomTab('logs')
      setError('')
      await refresh()
    } catch (reason) {
      const message = reason instanceof Error ? reason.message : String(reason)
      await refresh()
      setError(message)
    }
  }

  const openSimulationLog = async (test: RegressionTest) => {
    if (!test.latest.has_run) return
    setBottomTab('logs')
    setSimulationLogLoading(true)
    try {
      setSimulationLog(await api.simulationLog(test.id))
      setError('')
    } catch (reason) {
      setSimulationLog(null)
      setError(reason instanceof Error ? reason.message : String(reason))
    } finally {
      setSimulationLogLoading(false)
    }
  }

  const validateAction = async () => {
    try {
      if (workflow.validated) {
        await api.resetValidation()
        await refresh()
      } else {
        const result = await api.validate()
        await refresh()
        if (!result.success) setError(result.message)
      }
    } catch (reason) {
      setError(reason instanceof Error ? reason.message : String(reason))
    }
  }

  const compileAction = async () => {
    if (workflow.compiled) {
      try {
        await api.resetBuild()
        await refresh()
      } catch (reason) {
        setError(reason instanceof Error ? reason.message : String(reason))
      }
      return
    }
    await runAction('build')
  }

  if (loading) {
    return <div className="loading-screen"><img src="/api/project/logo" alt="Marlim3" /><span>{t('app.loading')}</span></div>
  }

  const analysisMessage = !selectedTests.length ? t('analysis.selectTest') : t('analysis.runTests')

  return <main className="app-shell">
    <header className="topbar">
      <div className="brand"><img src="/api/project/logo" alt="Marlim3" /><span>{t('app.title')}</span></div>
      <div className="separator" />
      <LanguageToggle />
      {workflow.ready && <div className="status-chip green"><Check size={14} /> {t('workflow.ready')}</div>}
      <div className="top-actions">
        <button className={workflow.validated ? 'confirmed' : ''} onClick={() => void validateAction()} disabled={job?.status === 'running'}>
          <Check size={18} /> {workflow.validated ? t('workflow.validated') : t('workflow.validate')}
        </button>
        <button className={workflow.compiled ? 'confirmed' : ''} onClick={() => void compileAction()} disabled={job?.status === 'running'}>
          <Hammer size={18} /> {workflow.compiled ? t('workflow.compiled') : t('workflow.compile')}
        </button>
      </div>
    </header>

    {error && <div className="error-banner"><span>{error}</span><button onClick={() => setError('')}>×</button></div>}

    <section className="workspace">
      <aside className="left-panel panel">
        <div className="panel-title">{t('tests.title')}</div>
        <div className="search-row">
          <Search size={17} />
          <input value={testSearch} onChange={event => setTestSearch(event.target.value)} placeholder={t('tests.search')} />
          <button><SlidersHorizontal size={17} /></button>
        </div>
        <div className="test-list">
          {filteredTests.map(test => <TestRow
            key={test.id}
            test={test}
            selected={selectedTests.includes(test.id)}
            live={liveRunItems.get(test.id)}
            onToggle={toggleTest}
            t={t}
          />)}
          {!filteredTests.length && <div className="empty-list">{t('tests.empty')}</div>}
        </div>
        <div className="groups">
          <span>{t('tests.groups')}</span>
          <div>{suites.map((suite, index) => <button key={suite} className={`group group-${index % 3}`}>{suite}</button>)}</div>
        </div>
        <div className="left-actions">
          <button onClick={() => setShowAdd(true)}><Plus size={20} /> {t('tests.add')}</button>
          <button
            className="run-selection"
            onClick={() => void runAction('run')}
            disabled={!workflow.ready || !selectedTests.length || job?.status === 'running'}
            title={!workflow.ready ? t('tests.runBlocked') : ''}
          ><Play size={18} fill="currentColor" /> {t('tests.run')}</button>
          <button className={comparisonMode ? 'comparison-active' : ''} onClick={() => setComparisonMode(value => !value)} disabled={!canCompare}>
            <GitCompare size={19} /> {comparisonMode ? t('tests.compareExit') : t('tests.compare')}
          </button>
        </div>
        <div className="test-summary">
          <span>{t('tests.selected', { count: selectedTests.length })}</span>
          <span>{t('tests.passFail', { passed, failed })}</span>
        </div>
      </aside>

      <section className="center-column">
        <div className="graph-panel panel">
          <div className="graph-header">
            <h1>{comparisonMode ? t('graph.titleComparison') : t('graph.title')}</h1>
            <label className="recursion-filter">{t('graph.recursion')}
              <select value={recursionDepth} onChange={event => setRecursionDepth(event.target.value === 'normal' ? 'normal' : Number(event.target.value))}>
                <option value="normal">{t('graph.recursionNormal')}</option>
                {Array.from({ length: graph?.max_recursion_depth || 0 }, (_, index) => index + 1).map(level => <option value={level} key={level}>{level}</option>)}
              </select>
            </label>
            <button className="icon-button"><MoreVertical size={19} /></button>
          </div>
          <div className="graph-filterbar">
            <div className="scope-tabs">
              <button className={scope === 'main' ? 'active' : ''} onClick={() => setScope('main')}>{t('graph.scopeMain')}</button>
              <button className={scope === 'all' ? 'active' : ''} onClick={() => setScope('all')}>{t('graph.scopeAll', { count: compact(graph?.nodes.length || 0) })}</button>
              <button className={scope === 'uncovered' ? 'active' : ''} onClick={() => setScope('uncovered')}>
                {t('graph.scopePending', { count: compact((graph?.status_counts.partial || 0) + (graph?.status_counts.uncovered || 0)) })}
              </button>
            </div>
            <form className="function-search" onSubmit={event => {
              event.preventDefault()
              if (graph?.nodes.some(node => node.label.toLowerCase().includes(functionInput.trim().toLowerCase()))) setScope('all')
              setFunctionSearch(functionInput)
            }}>
              <Search size={15} />
              <input value={functionInput} onChange={event => setFunctionInput(event.target.value)} placeholder={t('graph.searchFunction')} />
            </form>
          </div>
          <div className="graph-body">
            {graph && analysisAllowed && <GraphCanvas graph={graph} scope={scope} recursionDepth={recursionDepth} search={functionSearch} onSelect={selectNode} onVisibleCount={updateVisible} />}
            {(graphLoading || !graph) && <div className="graph-loading">{graphLoading ? <><span className="spinner" />{t('graph.loading')}</> : analysisMessage}</div>}
          </div>
          <div className="graph-footer">
            <span>{t('graph.footer', {
              nodes: compact(graph?.nodes.length || 0),
              visible: compact(visibleCount),
              edges: compact(graph?.edges.length || 0),
            })}</span>
            {comparisonMode && graph?.comparison
              ? <div className="legend comparison-legend">
                <span><i className="common" />{t('graph.legendCommon')}</span>
                <span><i className="left-only" />{graph.comparison.left.name}</span>
                <span><i className="right-only" />{graph.comparison.right.name}</span>
              </div>
              : <div className="legend">
                <span><i className="covered" />{t('graph.legendCovered', { count: compact(graph?.status_counts.covered || 0) })}</span>
                <span><i className="partial" />{t('graph.legendPartial', { count: compact(graph?.status_counts.partial || 0) })}</span>
                <span><i className="uncovered" />{t('graph.legendUncovered', { count: compact(graph?.status_counts.uncovered || 0) })}</span>
              </div>}
          </div>
        </div>

        <div className="code-panel panel">
          <div className="bottom-tabs">
            <button className={bottomTab === 'code' ? 'active' : ''} onClick={() => setBottomTab('code')}><Code2 size={15} /> {t('tabs.code')}</button>
            <button className={bottomTab === 'execution' ? 'active' : ''} onClick={() => setBottomTab('execution')}>{t('tabs.execution')}</button>
            <button className={bottomTab === 'validation' ? 'active' : ''} onClick={() => setBottomTab('validation')}>{t('tabs.validation')}</button>
            <button className={bottomTab === 'terminal' ? 'active' : ''} onClick={() => setBottomTab('terminal')}><Terminal size={15} /> {t('tabs.terminal')}</button>
            <button className={bottomTab === 'logs' ? 'active' : ''} onClick={() => setBottomTab('logs')}><FileText size={15} /> {t('tabs.logs')}</button>
          </div>
          {bottomTab === 'code' && <>
            <div className="code-toolbar">
              <span>{t('code.file')} <b>{analysisAllowed ? source?.relative_file || t('code.selectFunction') : t('code.blocked')}</b></span>
              <span className="code-percent">{analysisAllowed && selectedNode?.regions_total ? `${Math.round(100 * selectedNode.regions_covered / selectedNode.regions_total)}%` : ''}</span>
              <button disabled={!analysisAllowed}>{t('code.coverageLines')} <ChevronDown size={14} /></button>
              <MoreVertical size={17} />
            </div>
            <div className="source-view">{analysisAllowed && source
              ? source.lines.map(line => <div className={`source-line ${line.status}`} key={line.number}>
                <span className="hit-count">{line.count || ''}</span>
                <span className="line-number">{line.number}</span>
                <code>{line.text || ' '}</code>
              </div>)
              : <div className="empty-source"><Braces size={28} /> {analysisAllowed ? t('code.clickFunction') : analysisMessage}</div>}</div>
          </>}
          {bottomTab === 'execution' && <div className="runs-table">{(bootstrap?.tests || []).map(test => <button
            className={`run-row ${test.latest.has_run ? 'clickable' : ''}`}
            key={test.id}
            onClick={test.latest.has_run ? () => void openSimulationLog(test) : undefined}
            title={test.latest.has_run ? t('code.openLog') : t('code.notRunYet')}
          >
            <ResultStatusIcon status={test.latest.status} />
            <span>{test.name}</span><span>{test.latest.status}</span>
            <span>{test.latest.duration.toFixed(2)}s</span>
            <span>{test.latest.coverage.lines.percent.toFixed(1)}%</span>
          </button>)}</div>}
          {bottomTab === 'validation' && <div className="tab-message"><Check size={24} /> {t('validation.summary', {
            validated: workflow.validated ? t('validation.confirmed') : t('validation.pending'),
            compiled: workflow.compiled ? t('validation.confirmed') : t('validation.pending'),
          })}</div>}
          {bottomTab === 'terminal' && <div ref={terminalRef} className="terminal-output">{job?.kind === 'run' && job.terminal?.length
            ? job.terminal.map((entry, index) => <div className={`terminal-line ${entry.stream}`} key={`${entry.test_id}-${index}`}><span>{entry.text}</span></div>)
            : <div className="terminal-empty">{t('terminal.empty')}</div>}</div>}
          {bottomTab === 'logs' && (simulationLogLoading
            ? <div className="tab-message"><span className="spinner" />{t('logs.loading')}</div>
            : simulationLog
              ? <pre className="job-log simulation-log">{[
                t('logs.model', { name: simulationLog.name }),
                t('logs.status', { status: simulationLog.status }),
                t('logs.file', { file: simulationLog.file || t('logs.notGenerated') }),
                simulationLog.truncated ? t('logs.truncated') : '',
                '',
                simulationLog.content,
              ].filter((line, index) => Boolean(line) || index === 4).join('\n')}</pre>
              : <pre className="job-log">{job ? [
                job.kind === 'run' && job.progress ? t('logs.progress', { completed: job.progress.completed, total: job.progress.total }) : '',
                job.message,
                '',
                ...(job.log || []).map(item => item.text),
              ].filter((line, index) => Boolean(line) || index === 2).join('\n') : t('logs.placeholder')}</pre>)}
        </div>
      </section>

      <aside className="right-column">
        <div className="panel side-section">
          <button className="section-title" onClick={() => setCoverageOpen(value => !value)}>
            <span>{t('coverage.title')}</span>{coverageOpen ? <ChevronUp /> : <ChevronDown />}
          </button>
          {coverageOpen && (analysisAllowed && graph
            ? comparisonMode && graph.comparison ? <ComparisonCoverage graph={graph} /> : <div className="coverage-grid">
              <CoverageCard title={t('coverage.lines')} metric={summary?.lines || emptyMetric} tone="#43db62" format={compact} />
              <CoverageCard title={t('coverage.functions')} metric={summary?.functions || emptyMetric} tone="#43db62" format={compact} />
              <CoverageCard title={t('coverage.branches')} metric={summary?.branches || emptyMetric} tone="#f2b62d" format={compact} />
              <CoverageCard title={t('coverage.files')} metric={summary?.files || emptyMetric} tone="#3b9cff" format={compact} ratio />
            </div>
            : <div className="blocked-panel">{analysisMessage}</div>)}
        </div>
        <div className="panel side-section contributions">
          <button className="section-title" onClick={() => setContributionOpen(value => !value)}>
            <span>{comparisonMode ? t('contributions.comparisonTitle') : t('contributions.title')} <Info size={15} /></span>
            {contributionOpen ? <ChevronUp /> : <ChevronDown />}
          </button>
          {contributionOpen && (analysisAllowed && graph
            ? comparisonMode && graph.comparison ? <div className="comparison-details">
              <div><span>{t('comparison.commonLines')}</span><b>{compact(graph.comparison.delta.common_lines)}</b></div>
              <div><span>{t('comparison.onlyIn', { name: graph.comparison.left.name })}</span><b>{compact(graph.comparison.delta.left_only_lines)}</b></div>
              <div><span>{t('comparison.onlyIn', { name: graph.comparison.right.name })}</span><b>{compact(graph.comparison.delta.right_only_lines)}</b></div>
            </div>
              : <div>{(graph.contributions || []).map(item => <div className="contribution-row" key={item.test_id}>
                <span title={item.name}>{item.name}</span><b>+{item.line_points.toFixed(2)}%</b>
              </div>)}</div>
            : <div className="blocked-panel compact">{t('contributions.unavailable')}</div>)}
        </div>
        <div className="panel side-section details">
          <button className="section-title" onClick={() => setDetailsOpen(value => !value)}>
            <span>{t('details.title')}</span>{detailsOpen ? <ChevronUp /> : <ChevronDown />}
          </button>
          {detailsOpen && (analysisAllowed ? <div className="method-details">
            <h3 className={selectedNode?.status || ''}>{selectedNode?.short || t('details.selectFunction')}</h3>
            <dl>
              <dt>{t('details.file')}</dt><dd title={selectedNode?.file}>{selectedNode?.file?.split('/').slice(-3).join('/') || t('details.unavailable')}</dd>
              <dt>{t('details.calls')}</dt><dd>{compact(selectedNode?.count || 0)}</dd>
              <dt>{t('details.coverage')}</dt><dd>{selectedNode ? t('details.regions', { covered: selectedNode.regions_covered, total: selectedNode.regions_total }) : t('details.unavailable')}</dd>
              <dt>{t('details.tests')}</dt><dd>{selectedNode ? `${selectedNode.test_count}/${selectedNode.selected_count}` : t('details.unavailable')}</dd>
              <dt>{t('details.namespace')}</dt><dd>{selectedNode?.namespace || t('details.unavailable')}</dd>
            </dl>
          </div> : <div className="blocked-panel">{analysisMessage}</div>)}
        </div>
      </aside>
    </section>

    <footer className="statusbar">
      <span>{t('status.project', { root: bootstrap?.project.root || '' })}</span>
      <span>{t('status.lastRun', { time: formatTime(lastRun) })}</span>
      <span>{t('status.environment')} <Settings2 size={15} /></span>
    </footer>
    {showAdd && <AddTestDialog onClose={() => setShowAdd(false)} onSaved={() => void refresh()} />}
  </main>
}

export default function App() {
  return <LanguageProvider><RegressionApp /></LanguageProvider>
}
