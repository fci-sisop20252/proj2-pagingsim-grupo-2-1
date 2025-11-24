# Relatório do Projeto 2: Simulador de Memória Virtual

**Disciplina:** Sistemas Operacionais
**Professor:** Lucas Figueiredo
**Data:** 20/11/2025

## Integrantes do Grupo

- Caio Cesar Navarro Pugliese       RA: 10436413
- Erik Dong Kyu Kang                RA: 10439715
- Matheus Lemos Rosal do valle      RA: 10442011
- Rodrigo Daiske Uehara             RA: 10440295

---

## 1. Instruções de Compilação e Execução

### 1.1 Compilação

gcc -o simuPagi simuPagi.c

### 1.2 Execução

Forneça exemplos completos de como executar o simulador.

FIFO:

./simuPagi fifo tests/config_1.txt tests/acessos_1.txt
./simuPagi fifo tests/config_2.txt tests/acessos_2.txt
./simuPagi fifo tests/config_3.txt tests/acessos_3.txt
./simuPagi fifo tests/config_4.txt tests/acessos_4.txt
./simuPagi fifo tests/config_5.txt tests/acessos_5.txt
./simuPagi fifo tests/config_6.txt tests/acessos_6.txt


CLOCK:

./simuPagi clock tests/config_1.txt tests/acessos_1.txt
./simuPagi clock tests/config_2.txt tests/acessos_2.txt
./simuPagi clock tests/config_3.txt tests/acessos_3.txt
./simuPagi clock tests/config_4.txt tests/acessos_4.txt
./simuPagi clock tests/config_5.txt tests/acessos_5.txt
./simuPagi clock tests/config_6.txt tests/acessos_6.txt

---

## 2. Decisões de Design

### 2.1 Estruturas de Dados

Descreva as estruturas de dados que você escolheu para representar:

**Tabela de Páginas:**
- Qual estrutura usou? (array, lista, hash map, etc.)
  
  Matrizes e arrays
  
- Quais informações armazena para cada página?
  
  paginaCarregada[i][j]: se a página j do processo i está na memória ou não.
  framePagina[i][j]: índice do frame físico onde a página está carregada.
  R_Pagina[i][j]: bit de referência usado no algoritmo Clock.
  
- Como organizou para múltiplos processos?
  
  A primeira dimensão é o índice do processo. idProcesso contém o PID real e a busca de índice é feita por
  buscaProcesso(id).
   
- **Justificativa:** Por que escolheu essa abordagem?

  Matrizes fixas são simples, rápidas para checar presença, recuperar frame e atualizar R-bit e para simulações é prático e evita overhead
  de estruturas dinâmicas.

**Frames Físicos:**
- Como representou os frames da memória física?

  idFrame[MAX_FRAMES]  PID do processo dono da página naquele frame.
  numPagina_Frame[MAX_FRAMES]  número da página carregada naquele frame.
  frameLivre[MAX_FRAMES] 1 = livre, 0 = ocupado.
  
- Quais informações armazena para cada frame?

  Identificador do processo (idFrame), número da página (numPagina_Frame) e flag livre/ocupado (frameLivre).
  
- Como rastreia frames livres vs ocupados?

  frameLivre inteiro serve para buscar um frame livre com buscarFrameLivre() (varre frameLivre[i] == 1) e marcar frameLivre[f] = 0 ao
  alocar.
  
- **Justificativa:** Por que escolheu essa abordagem?

  Representação por arrays é direta e permite localizar rapidamente qual página está em cada frame, mantém
  correspondência simples entre tabela de páginas e frames físicos e permite implementar facilmente algoritmos de substituição que precisam
  inspecionar informações por frame.
  
**Estrutura para FIFO:**
- Como mantém a ordem de chegada das páginas?

  Um ponteiro circular posiFifo que aponta para o próximo frame vítima, a cada substituição selecionaVitimaFIFO() retorna posiFifo e
  atualiza o posiFifo = (posiFifo + 1) % nFrame.
  
- Como identifica a página mais antiga?

  A própria posição do ponteiro circular indica o frame que foi carregado há mais tempo (FIFO simples). Não se guarda
  timestamp explícito — o avanço rotativo preserva ordem.
  
- **Justificativa:** Por que escolheu essa abordagem?

  Porque é mais simples e eficiênte apenas um inteiro posiFifo e incremento modular e para simular FIFO sem necessidade de remover
  itens do meio, isto é suficiente por substituição, e também evita estruturas mais pesadas porque o estado do sistema
  já está mantido por arrays.

**Estrutura para Clock:**
- Como implementou o ponteiro circular?

  posiClock é um índice que é avançado circularmente (posiClock = (posiClock + 1) % nFrame) dentro de selecionaVitimaClock() até encontrar    uma vítima.
  
- Como armazena e atualiza os R-bits?

  Os R-bits estão em R_Pagina[proc_index][pagina], quando há acesso com HIT, a função clock() faz R_Pagina[idProc][pagina] = 1.
  Ao inspecionar um frame candidato, selecionaVitimaClock() consulta R_Pagina[idProc][numPagina]

  Se R == 0  escolhe o frame como vítima.
  Se R == 1  zera R_Pagina = 0 e avança o ponteiro, dando "segunda chance".
  
- **Justificativa:** Por que escolheu essa abordagem?

  Mantendo R-bit por entrada de página permite localizar o R correspondente ao conteúdo de cada frame via
  idFrame e numPagina_Frame.

### 2.2 Organização do Código

Descreva como organizou seu código:

- Quantos arquivos/módulos criou?
- Qual a responsabilidade de cada arquivo/módulo?
- Quais são as principais funções e o que cada uma faz?

simuPagi.c
├── main() - Lê argumentos, abre arquivos de configuração e acessos, inicializa estruturas e faz o loop de simulação.
├── buscaProcesso(int id) - Retorna o índice do processo a partir do PID (idProcesso[i] == id).
├── buscarFrameLivre() - Varre frameLivre[] e retorna índice de frame livre ou -1.
├── selecionaVitimaFIFO() - Retorna posiFifo e avança o ponteiro.
├── selecionaVitimaClock() - Implementa busca circular com segunda chance (usa R_Pagina).
├── fifo(int id, int ende) - Simula um acesso usando FIFO: detecta HIT, aloca em frame livre, ou substitui via FIFO.
├── clock(int id, int ende) - Simula um acesso usando Clock: atualiza R-bit em HIT; ao alocar em frame livre seta R=1; ao substituir, zera R-bit da antiga e seta R da nova.

### 2.3 Algoritmo FIFO

Explique **como** implementou a lógica FIFO:

- Como mantém o controle da ordem de chegada?

  Uso de um ponteiro circular (posiFifo) que percorre os frames em ordem cíclica. Ao inserir uma página em memória,
  o ponteiro indica qual frame ocupa-se como próxima vítima.
  
- Como seleciona a página vítima?

  selecionaVitimaFIFO() retorna o frame apontado por posiFifo e avança posiFifo em 1. Aquele frame é considerado a página
  mais antiga.
  
- Quais passos executa ao substituir uma página?

  Ao detectar page fault e não haver frame livre:
  Identificar o frame vítima via posiFifo.
  Ler idFrame[vitima] e numPagina_Frame[vitima] para saber qual entrada da tabela de páginas limpar.
  Marcar a página antiga como não carregada.
  Atualizar idFrame[vitima] e numPagina_Frame[vitima] com o novo processo/página.
  Marcar a nova página como carregada e registrar framePagina[id][pagina] = vitima.
  Incrementar contadores de page fault.

**Não cole código aqui.** Explique a lógica em linguagem natural.

### 2.4 Algoritmo Clock

Explique **como** implementou a lógica Clock:

- Como gerencia o ponteiro circular?

  Mantém um índice posiClock que aponta para o próximo frame a inspecionar, ele é avançado circularmente até encontrar uma vítima.
  
- Como implementou a "segunda chance"?

  Para o frame apontado, obter o PID e o número da página e mapear para o índice do processo,então ler R_Pagina[idProc][pagina].
  Se R == 0 escolhe esse frame como vítima.
  Se R == 1 zera R_Pagina = 0 (retira a "segunda chance" e avança o ponteiro, continuando a busca. Assim a página recebe uma
  "segunda chance" e só será removida na próxima volta se não for referenciada novamente.
  
- Como trata o caso onde todas as páginas têm R=1?

  O ponteiro fará uma passagem completa; em cada visita encontrará R==1 e as zera, ao completar a volta, haverá frames com R==0 e um
  deles será escolhido, a primeira volta transforma todos os R em 0; na volta seguinte encontra uma vítima.
  
- Como garante que o R-bit é setado em todo acesso?

  Em cada acesso com HIT, o código define R_Pagina[idProc][pagina] = 1. Ao carregar uma página em frame livre ou por substituição, o R-bit
  da nova página é inicializado para 1 e Ao descartar página, zera seu R-bit e marca paginaCarregada = 0.

**Não cole código aqui.** Explique a lógica em linguagem natural.

### 2.5 Tratamento de Page Fault

Explique como seu código distingue e trata os dois cenários:

**Cenário 1: Frame livre disponível**
- Como identifica que há frame livre?

  buscarFrameLivre() varre frameLivre[] e retorna índice f se frameLivre[f] == 1 e se f != -1 há frame livre.
  
- Quais passos executa para alocar a página?

  frameLivre[f] = 0.
  idFrame[f] = id e numPagina_Frame[f] = pagina.
  paginaCarregada[idProc][pagina] = 1 e framePagina[idProc][pagina] = f.
  No Clock, setar R_Pagina[idProc][pagina] = 1.
  Incrementar total_faults e registrar informação de log.

**Cenário 2: Memória cheia (substituição)**
- Como identifica que a memória está cheia?
  
  buscarFrameLivre() retorna -1 → nenhum frame livre.
  
- Como decide qual algoritmo usar (FIFO vs Clock)?

  O main() lê o primeiro argumento (argv[1]) — "fifo" ou "clock". No loop de acessos, chama fifo(id, ende) ou clock(id, ende) conforme
  strcmp(alg, "fifo") / strcmp(alg, "clock").
  
- Quais passos executa para substituir uma página?

  Escolher vítima:
    FIFO: vitima = selecionaVitimaFIFO().
    Clock: vitima = selecionaVitimaClock().
  Desassociar página antiga:
    Recuperar idAntigo = idFrame[vitima] e paginaAntiga = numPagina_Frame[vitima].
    Determinar índice do processo antigo idProcAntigo = buscaProcesso(idAntigo).
    paginaCarregada[idProcAntigo][paginaAntiga] = 0.
    No Clock também R_Pagina[idProcAntigo][paginaAntiga] = 0.

  Colocar nova página no frame vítima:
    idFrame[vitima] = id, numPagina_Frame[vitima] = pagina,
    paginaCarregada[idProc][pagina] = 1, framePagina[idProc][pagina] = vitima,
    No Clock: R_Pagina[idProc][pagina] = 1.

  Atualizar estatísticas:
     incrementar total_faults.

---

## 3. Análise Comparativa FIFO vs Clock

### 3.1 Resultados dos Testes

Preencha a tabela abaixo com os resultados de pelo menos 3 testes diferentes:

| Descrição do Teste | Total de Acessos | Page Faults FIFO | Page Faults Clock | Diferença |
|-------------------|------------------|------------------|-------------------|-----------|
| Teste 1 - Básico  |                  |                  |                   |           |
| Teste 2 - Memória Pequena |          |                  |                   |           |
| Teste 3 - Simples |                  |                  |                   |           |
| Teste Próprio 1   |                  |                  |                   |           |

### 3.2 Análise

Com base nos resultados acima, responda:

1. **Qual algoritmo teve melhor desempenho (menos page faults)?**

2. **Por que você acha que isso aconteceu?** Considere:
   - Como cada algoritmo escolhe a vítima
   - O papel do R-bit no Clock
   - O padrão de acesso dos testes

3. **Em que situações Clock é melhor que FIFO?**
   - Dê exemplos de padrões de acesso onde Clock se beneficia

4. **Houve casos onde FIFO e Clock tiveram o mesmo resultado?**
   - Por que isso aconteceu?

5. **Qual algoritmo você escolheria para um sistema real e por quê?**

---

## 4. Desafios e Aprendizados

### 4.1 Maior Desafio Técnico

Descreva o maior desafio técnico que seu grupo enfrentou durante a implementação:

- Qual foi o problema?

   Como começar a codar, nao sabiamos como começar o codigo.

- Como identificaram o problema?

   Ao tentarmos iniciar o projeto nao saimos do lugar ficamos com um vazio na cabeça.

- Como resolveram?

   Resolvemos ceder e perguntar ao Gemini por onde poderiamos começar, e a partir da resposta dele conseguimos proceder.

- O que aprenderam com isso?

   Que a parte mais dificil de programar algo do zero mesmo sabendo a logica e o que queremos fazer é iniciar o codigo.

### 4.2 Principal Aprendizado

Descreva o principal aprendizado sobre gerenciamento de memória que vocês tiveram com este projeto:

- O que vocês não entendiam bem antes e agora entendem?

   Como funciona a paginação e a substituição na tabela de paginas.

- Como este projeto mudou sua compreensão de memória virtual?

   Ele mudou meu entendimento de como ela é organizada e gerida.

- Que conceito das aulas ficou mais claro após a implementação?

   A paginação ficou mais clara, alem do funcionamento do CLOCK.

---

## 5. Vídeo de Demonstração

**Link do vídeo:** [Insira aqui o link para YouTube, Google Drive, etc.]

### Conteúdo do vídeo:

Confirme que o vídeo contém:

- [ ] Demonstração da compilação do projeto
- [ ] Execução do simulador com algoritmo FIFO
- [ ] Execução do simulador com algoritmo Clock
- [ ] Explicação da saída produzida
- [ ] Comparação dos resultados FIFO vs Clock
- [ ] Breve explicação de uma decisão de design importante

---

## Checklist de Entrega

Antes de submeter, verifique:

- [ ] Código compila sem erros conforme instruções da seção 1.1
- [ ] Simulador funciona corretamente com FIFO
- [ ] Simulador funciona corretamente com Clock
- [ ] Formato de saída segue EXATAMENTE a especificação do ENUNCIADO.md
- [ ] Testamos com os casos fornecidos em tests/
- [ ] Todas as seções deste relatório foram preenchidas
- [ ] Análise comparativa foi realizada com dados reais
- [ ] Vídeo de demonstração foi gravado e link está funcionando
- [ ] Todos os integrantes participaram e concordam com a submissão

---
## Referências

Gemini: https://gemini.google.com/share/d45e35cd1e2d
Sobre argc e argv: https://linguagemc.com.br/argumentos-em-linha-de-comando/
Logica geral de paginação: https://www.canva.com/design/DAG2-rwIcL0/S9Nu66vqoRgZcWFkelYRLQ/edit
Logica do CLOCK: https://www.canva.com/design/DAG3A6vEFAM/HYeVocOX1Onnm6Q7WOSenw/edit

---

## Comentários Finais

Use este espaço para quaisquer observações adicionais que julguem relevantes (opcional).

---
