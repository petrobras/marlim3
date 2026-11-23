import type { BatchResult, Bootstrap, GraphData, Job, ModelFile, SimulationLog, SourceData, ValidationResult } from './types'

async function request<T>(path: string, init?: RequestInit): Promise<T> {
  const response = await fetch(path, {
    ...init,
    headers: { 'Content-Type': 'application/json', ...(init?.headers || {}) },
  })
  const payload = await response.json()
  if (!response.ok) throw new Error(payload.error || `HTTP request failed (${response.status})`)
  return payload as T
}

const selectedQuery = (ids: string[]) => `test_ids=${encodeURIComponent(ids.join(','))}`

export const api = {
  bootstrap: () => request<Bootstrap>('/api/bootstrap'),
  graph: (ids: string[]) => request<GraphData>(`/api/graph?${selectedQuery(ids)}`),
  source: (file: string, line: number, endLine: number, ids: string[]) =>
    request<SourceData>(`/api/source?file=${encodeURIComponent(file)}&line=${line}&end_line=${endLine}&${selectedQuery(ids)}`),
  validate: () => request<ValidationResult>('/api/validate', { method: 'POST', body: '{}' }),
  resetValidation: () => request('/api/validate/reset', { method: 'POST', body: '{}' }),
  build: () => request<Job>('/api/build', { method: 'POST', body: '{}' }),
  resetBuild: () => request('/api/build/reset', { method: 'POST', body: '{}' }),
  run: (ids: string[]) => request<Job>('/api/run', { method: 'POST', body: JSON.stringify({ test_ids: ids }) }),
  compare: (ids: string[]) => request<GraphData>('/api/compare', { method: 'POST', body: JSON.stringify({ test_ids: ids }) }),
  job: (id: string) => request<Job>(`/api/jobs/${id}`),
  simulationLog: (testId: string) => request<SimulationLog>(`/api/tests/${encodeURIComponent(testId)}/simulation-log`),
  saveTest: (test: Record<string, unknown>) => request('/api/tests', { method: 'POST', body: JSON.stringify(test) }),
  saveTestsBatch: (models: ModelFile[], suite: string) =>
    request<BatchResult>('/api/tests/batch', { method: 'POST', body: JSON.stringify({ model_files: models, suite }) }),
  deleteTest: (id: string) => request(`/api/tests/${id}`, { method: 'DELETE' }),
}
