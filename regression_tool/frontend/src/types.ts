export type CoverageMetric = { covered: number; total: number; percent: number }
export type CoverageSummary = {
  lines: CoverageMetric
  functions: CoverageMetric
  branches: CoverageMetric
  files: CoverageMetric
}

export type RegressionTest = {
  id: string
  name: string
  model_path: string
  suite: string
  description: string
  active: boolean
  tags: string[]
  latest: {
    status: string
    raw_status: string
    run_id: string
    started_at: string
    duration: number
    coverage: CoverageSummary
    has_run: boolean
    has_coverage: boolean
  }
}

export type Bootstrap = {
  project: { name: string; root: string }
  detection: { valid_project: boolean; recommended_backend: string; tools: { name: string; available: boolean }[] }
  build_ready: boolean
  workflow: { validated: boolean; compiled: boolean; ready: boolean }
  tests: RegressionTest[]
  summary: CoverageSummary
  generated_at: string
}

export type GraphNode = {
  id: string
  label: string
  short: string
  file: string
  line: number
  end_line: number
  hit_tests: string[]
  test_count: number
  selected_count: number
  count: number
  status: 'covered' | 'partial' | 'uncovered'
  regions_covered: number
  regions_total: number
  namespace: string
  directory: string
  level: number
  recursion_depth: number | null
  comparison?: 'common' | 'left_only' | 'right_only' | 'neither'
}

export type GraphEdge = { source: string; target: string; approximate: boolean; covered: boolean }
export type Contribution = {
  test_id: string
  name: string
  added_lines: number
  added_functions: number
  line_points: number
  redundancy_percent: number
}

export type GraphData = {
  nodes: GraphNode[]
  edges: GraphEdge[]
  summary: CoverageSummary
  status_counts: Record<'covered' | 'partial' | 'uncovered', number>
  contributions: Contribution[]
  selected_tests: { id: string; name: string }[]
  max_recursion_depth: number
  comparison?: {
    left: { id: string; name: string; summary: CoverageSummary }
    right: { id: string; name: string; summary: CoverageSummary }
    delta: {
      common_functions: string[]
      left_only_functions: string[]
      right_only_functions: string[]
      common_lines: number
      left_only_lines: number
      right_only_lines: number
      line_percent_delta: number
    }
  }
  complete: boolean
  generated_at: string
}

export type SourceData = {
  file: string
  relative_file: string
  start: number
  end: number
  lines: { number: number; text: string; count: number; status: 'covered' | 'partial' | 'uncovered' | 'neutral' }[]
}

/** Live state of one queued model while a run job progresses. */
export type JobItem = {
  test_id: string
  name: string
  state: 'pending' | 'running' | 'completed' | 'error'
  result_status: string
  has_run: boolean
  has_coverage: boolean
  duration: number
  exit_code: number | null
}

export type Job = {
  id: string
  kind: 'build' | 'run'
  status: 'running' | 'passed' | 'failed'
  message: string
  log: { stream: string; text: string }[]
  started_at: string
  finished_at: string
  progress?: { completed: number; total: number; current_test_id: string | null }
  terminal?: { test_id: string; name: string; stream: 'stdout' | 'stderr' | 'status'; text: string }[]
  items?: JobItem[]
  result?: {
    test_id: string
    name: string
    status: string
    has_run: boolean
    has_coverage: boolean
    duration: number
    exit_code: number | null
  }[]
}

export type SimulationLog = {
  test_id: string
  name: string
  status: 'passed' | 'crashed'
  available: boolean
  file: string
  content: string
  truncated: boolean
}

export type ValidationResult = {
  success: boolean
  message: string
  workflow: Bootstrap['workflow']
}

/** A model file encoded for upload; the test name is derived from `name`. */
export type ModelFile = {
  name: string
  content_base64: string
}

export type BatchResult = {
  created: RegressionTest[]
  count: number
}
