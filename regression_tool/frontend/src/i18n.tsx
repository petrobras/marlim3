import { createContext, useCallback, useContext, useEffect, useMemo, useState, type ReactNode } from 'react'

export type Language = 'en' | 'pt'

const STORAGE_KEY = 'marlim3-regression-tool.language'
export const DEFAULT_LANGUAGE: Language = 'en'

/** Every user-facing string, English first because English is the default. */
const dictionary = {
  en: {
    'app.loading': 'Loading Regression Tool...',
    'app.title': 'Regression Tool',
    'language.label': 'Language',
    'language.en': 'English',
    'language.pt': 'Português',

    'workflow.ready': 'Ready',
    'workflow.validate': 'Validate',
    'workflow.validated': 'Validated',
    'workflow.compile': 'Build',
    'workflow.compiled': 'Built',

    'tests.title': 'Tests',
    'tests.search': 'Search test',
    'tests.empty': 'No tests registered',
    'tests.groups': 'Groups',
    'tests.add': 'Add',
    'tests.run': 'Run selection',
    'tests.compare': 'Compare',
    'tests.compareExit': 'Exit comparison',
    'tests.selected': '{count} tests selected',
    'tests.passFail': '{passed} pass / {failed} fail',
    'tests.runBlocked': 'Validate and build the project first',
    'tests.statusRunning': 'Running now',
    'tests.statusFailed': 'Failure: {status}',
    'tests.statusPassed': 'Ran successfully',
    'tests.statusPending': 'Not run yet',

    'graph.title': 'Execution flow',
    'graph.titleComparison': 'Flow comparison',
    'graph.recursion': 'Recursion levels',
    'graph.recursionNormal': 'Normal',
    'graph.scopeMain': 'Main flow',
    'graph.scopeAll': 'All ({count})',
    'graph.scopePending': 'Pending ({count})',
    'graph.searchFunction': 'Find function',
    'graph.loading': 'Loading the complete graph...',
    'graph.footer': '{nodes} functions loaded · showing {visible} · {edges} calls',
    'graph.legendCovered': 'Covered ({count})',
    'graph.legendPartial': 'Partial ({count})',
    'graph.legendUncovered': 'Not covered ({count})',
    'graph.legendCommon': 'Common',
    'graph.fit': 'Fit to screen',
    'graph.zoomIn': 'Zoom in',
    'graph.zoomOut': 'Zoom out',
    'graph.center': 'Center',
    'graph.controls': 'Graph controls',

    'tabs.code': 'Code',
    'tabs.execution': 'Execution',
    'tabs.validation': 'Validation',
    'tabs.terminal': 'Terminal',
    'tabs.logs': 'Logs',

    'code.file': 'File:',
    'code.selectFunction': 'Select a function in the graph',
    'code.blocked': 'Analysis blocked',
    'code.coverageLines': 'Coverage: Lines',
    'code.clickFunction': 'Click a function to open the covered code.',
    'code.openLog': 'Open simulacao.log',
    'code.notRunYet': 'Not run yet',
    'validation.summary': 'Project validation: {validated}. Build: {compiled}.',
    'validation.confirmed': 'confirmed',
    'validation.pending': 'pending',
    'terminal.empty': 'Program output will appear here during execution.',
    'logs.loading': 'Loading simulacao.log...',
    'logs.model': 'Model: {name}',
    'logs.status': 'Status: {status}',
    'logs.file': 'File: {file}',
    'logs.notGenerated': 'not generated',
    'logs.truncated': 'Showing only the last 5 MB of the file.',
    'logs.progress': 'Progress: {completed}/{total}',
    'logs.placeholder': 'Select a run to open its simulacao.log file.',

    'coverage.title': 'Coverage',
    'coverage.lines': 'Lines',
    'coverage.functions': 'Functions',
    'coverage.branches': 'Branches',
    'coverage.files': 'Files',
    'contributions.title': 'Test contribution',
    'contributions.comparisonTitle': 'Comparison differences',
    'contributions.unavailable': 'Analysis unavailable.',
    'comparison.commonLines': 'Common lines',
    'comparison.onlyIn': 'Only {name}',
    'comparison.lineDelta': 'Line difference: {value} pp',
    'details.title': 'Method details',
    'details.selectFunction': 'Select a function',
    'details.file': 'File:',
    'details.calls': 'Calls:',
    'details.coverage': 'Coverage:',
    'details.tests': 'Tests:',
    'details.namespace': 'Namespace:',
    'details.regions': '{covered}/{total} regions',
    'details.unavailable': 'Not available',

    'analysis.selectTest': 'Select at least one executed test to view the analysis.',
    'analysis.runTests': 'Run every selected test to unlock flow, code and coverage.',

    'status.project': 'Project: {root}',
    'status.lastRun': 'Last run: {time}',
    'status.notRun': 'Not run yet',
    'status.environment': 'Environment: Debug x64',

    'add.title': 'Add models',
    'add.dropHere': 'Drag models here',
    'add.dropHint': 'or click to browse your computer',
    'add.selectedOne': '1 model selected',
    'add.selectedMany': '{count} models selected',
    'add.chooseOther': 'Click to choose other files',
    'add.nameHint': 'Each test is named after its model file.',
    'add.group': 'Group',
    'add.cancel': 'Cancel',
    'add.save': 'Save',
    'add.saving': 'Saving...',
    'add.remove': 'Remove',
    'add.error': 'Select at least one .mr3 or .json model',
  },
  pt: {
    'app.loading': 'Carregando Regression Tool...',
    'app.title': 'Regression Tool',
    'language.label': 'Idioma',
    'language.en': 'English',
    'language.pt': 'Português',

    'workflow.ready': 'Pronto',
    'workflow.validate': 'Validar',
    'workflow.validated': 'Validado',
    'workflow.compile': 'Compilar',
    'workflow.compiled': 'Compilado',

    'tests.title': 'Testes',
    'tests.search': 'Buscar teste',
    'tests.empty': 'Nenhum teste cadastrado',
    'tests.groups': 'Grupos',
    'tests.add': 'Adicionar',
    'tests.run': 'Executar seleção',
    'tests.compare': 'Comparar',
    'tests.compareExit': 'Sair da comparação',
    'tests.selected': '{count} testes selecionados',
    'tests.passFail': '{passed} passam / {failed} falham',
    'tests.runBlocked': 'Valide e compile o projeto primeiro',
    'tests.statusRunning': 'Executando agora',
    'tests.statusFailed': 'Falha: {status}',
    'tests.statusPassed': 'Executado com sucesso',
    'tests.statusPending': 'Ainda não executado',

    'graph.title': 'Fluxo de execução',
    'graph.titleComparison': 'Comparação do fluxo',
    'graph.recursion': 'Níveis de recursão',
    'graph.recursionNormal': 'Normal',
    'graph.scopeMain': 'Fluxo principal',
    'graph.scopeAll': 'Todas ({count})',
    'graph.scopePending': 'Pendentes ({count})',
    'graph.searchFunction': 'Localizar função',
    'graph.loading': 'Carregando grafo completo...',
    'graph.footer': '{nodes} funções carregadas · exibindo {visible} · {edges} chamadas',
    'graph.legendCovered': 'Coberto ({count})',
    'graph.legendPartial': 'Parcial ({count})',
    'graph.legendUncovered': 'Não coberto ({count})',
    'graph.legendCommon': 'Comum',
    'graph.fit': 'Ajustar à tela',
    'graph.zoomIn': 'Ampliar',
    'graph.zoomOut': 'Reduzir',
    'graph.center': 'Centralizar',
    'graph.controls': 'Controles do grafo',

    'tabs.code': 'Código',
    'tabs.execution': 'Execução',
    'tabs.validation': 'Validação',
    'tabs.terminal': 'Terminal',
    'tabs.logs': 'Logs',

    'code.file': 'Arquivo:',
    'code.selectFunction': 'Selecione uma função no grafo',
    'code.blocked': 'Análise bloqueada',
    'code.coverageLines': 'Cobertura: Linhas',
    'code.clickFunction': 'Clique em uma função para abrir o código coberto.',
    'code.openLog': 'Abrir simulacao.log',
    'code.notRunYet': 'Ainda não executado',
    'validation.summary': 'Validação do projeto: {validated}. Compilação: {compiled}.',
    'validation.confirmed': 'confirmada',
    'validation.pending': 'pendente',
    'terminal.empty': 'A saída do programa aparecerá aqui durante a execução.',
    'logs.loading': 'Carregando simulacao.log...',
    'logs.model': 'Modelo: {name}',
    'logs.status': 'Status: {status}',
    'logs.file': 'Arquivo: {file}',
    'logs.notGenerated': 'não gerado',
    'logs.truncated': 'Exibindo apenas os últimos 5 MB do arquivo.',
    'logs.progress': 'Progresso: {completed}/{total}',
    'logs.placeholder': 'Selecione uma execução para abrir o arquivo simulacao.log.',

    'coverage.title': 'Cobertura',
    'coverage.lines': 'Linhas',
    'coverage.functions': 'Funções',
    'coverage.branches': 'Branches',
    'coverage.files': 'Arquivos',
    'contributions.title': 'Contribuição dos testes',
    'contributions.comparisonTitle': 'Diferenças da comparação',
    'contributions.unavailable': 'Análise indisponível.',
    'comparison.commonLines': 'Linhas comuns',
    'comparison.onlyIn': 'Somente {name}',
    'comparison.lineDelta': 'Diferença de linhas: {value} p.p.',
    'details.title': 'Detalhes do método',
    'details.selectFunction': 'Selecione uma função',
    'details.file': 'Arquivo:',
    'details.calls': 'Chamadas:',
    'details.coverage': 'Cobertura:',
    'details.tests': 'Testes:',
    'details.namespace': 'Namespace:',
    'details.regions': '{covered}/{total} regiões',
    'details.unavailable': 'Não disponível',

    'analysis.selectTest': 'Selecione ao menos um teste executado para visualizar a análise.',
    'analysis.runTests': 'Execute todos os testes selecionados para liberar fluxo, código e cobertura.',

    'status.project': 'Projeto: {root}',
    'status.lastRun': 'Última execução: {time}',
    'status.notRun': 'Ainda não executado',
    'status.environment': 'Ambiente: Debug x64',

    'add.title': 'Adicionar modelos',
    'add.dropHere': 'Arraste os modelos aqui',
    'add.dropHint': 'ou clique para buscar no computador',
    'add.selectedOne': '1 modelo selecionado',
    'add.selectedMany': '{count} modelos selecionados',
    'add.chooseOther': 'Clique para escolher outros arquivos',
    'add.nameHint': 'Cada teste recebe o nome do seu arquivo de modelo.',
    'add.group': 'Grupo',
    'add.cancel': 'Cancelar',
    'add.save': 'Salvar',
    'add.saving': 'Salvando...',
    'add.remove': 'Remover',
    'add.error': 'Selecione ao menos um modelo .mr3 ou .json',
  },
} as const

export type TranslationKey = keyof (typeof dictionary)['en']
export type Translate = (key: TranslationKey, values?: Record<string, string | number>) => string

type LanguageContextValue = {
  language: Language
  setLanguage: (next: Language) => void
  t: Translate
  locale: string
}

const LanguageContext = createContext<LanguageContextValue | null>(null)

function readStoredLanguage(): Language {
  try {
    const stored = window.localStorage.getItem(STORAGE_KEY)
    return stored === 'pt' || stored === 'en' ? stored : DEFAULT_LANGUAGE
  } catch {
    return DEFAULT_LANGUAGE
  }
}

function interpolate(template: string, values?: Record<string, string | number>): string {
  if (!values) return template
  return template.replace(/\{(\w+)\}/g, (match, key: string) =>
    key in values ? String(values[key]) : match)
}

export function LanguageProvider({ children }: { children: ReactNode }) {
  const [language, setLanguageState] = useState<Language>(readStoredLanguage)

  const setLanguage = useCallback((next: Language) => {
    setLanguageState(next)
    try {
      window.localStorage.setItem(STORAGE_KEY, next)
    } catch {
      // A restricted browser profile simply falls back to the session default.
    }
  }, [])

  useEffect(() => {
    document.documentElement.lang = language === 'pt' ? 'pt-BR' : 'en'
  }, [language])

  const value = useMemo<LanguageContextValue>(() => ({
    language,
    setLanguage,
    locale: language === 'pt' ? 'pt-BR' : 'en-US',
    t: (key, values) => interpolate(dictionary[language][key] ?? dictionary.en[key] ?? key, values),
  }), [language, setLanguage])

  return <LanguageContext.Provider value={value}>{children}</LanguageContext.Provider>
}

export function useLanguage(): LanguageContextValue {
  const value = useContext(LanguageContext)
  if (!value) throw new Error('useLanguage must be used inside a LanguageProvider')
  return value
}
