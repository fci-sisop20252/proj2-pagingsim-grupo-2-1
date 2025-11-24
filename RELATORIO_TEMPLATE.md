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
  R: Matrizes e arrays
  
- Quais informações armazena para cada página?
  
  paginaCarregada[i][j]: se a página j do processo i está na memória (1) ou não (0).
  framePagina[i][j]: índice do frame físico onde a página está carregada (ou -1).
  R_Pagina[i][j]: bit de referência usado no algoritmo Clock.
  
- Como organizou para múltiplos processos?
  
  A primeira dimensão é o índice do processo. idProcesso[i] contém o PID real; a busca de índice é feita por
  buscaProcesso(id).
   
- **Justificativa:** Por que escolheu essa abordagem?

  Matrizes fixas são simples, rápidas e permitem acesso O(1) para checar presença, recuperar frame e atualizar R-bit e
  o trade-off é consumo de memória (espaço estático Max_PROCESSOS * MAX_PAGINAS), mas nas configurações típicas de simulação isto é
  aceitável e facilita inicialização.

**Frames Físicos:**
- Como representou os frames da memória física?

  idFrame[MAX_FRAMES] → PID do processo dono da página naquele frame (ou -1).
  numPagina_Frame[MAX_FRAMES] → número da página carregada naquele frame (ou -1).
  frameLivre[MAX_FRAMES] → 1 = livre, 0 = ocupado.
  
- Quais informações armazena para cada frame?
- Como rastreia frames livres vs ocupados?
- **Justificativa:** Por que escolheu essa abordagem?

**Estrutura para FIFO:**
- Como mantém a ordem de chegada das páginas?
- Como identifica a página mais antiga?
- **Justificativa:** Por que escolheu essa abordagem?

**Estrutura para Clock:**
- Como implementou o ponteiro circular?
- Como armazena e atualiza os R-bits?
- **Justificativa:** Por que escolheu essa abordagem?

### 2.2 Organização do Código

Descreva como organizou seu código:

- Quantos arquivos/módulos criou?
- Qual a responsabilidade de cada arquivo/módulo?
- Quais são as principais funções e o que cada uma faz?

**Exemplo:**
```
simulador.c
├── main() - lê argumentos e coordena execução
├── ler_config() - processa arquivo de configuração
├── processar_acessos() - loop principal de simulação
├── traduzir_endereco() - calcula página e deslocamento
├── consultar_tabela() - verifica se página está na memória
├── tratar_page_fault() - lida com page faults
├── algoritmo_fifo() - seleciona vítima usando FIFO
└── algoritmo_clock() - seleciona vítima usando Clock
```

### 2.3 Algoritmo FIFO

Explique **como** implementou a lógica FIFO:

- Como mantém o controle da ordem de chegada?
- Como seleciona a página vítima?
- Quais passos executa ao substituir uma página?

**Não cole código aqui.** Explique a lógica em linguagem natural.

### 2.4 Algoritmo Clock

Explique **como** implementou a lógica Clock:

- Como gerencia o ponteiro circular?
- Como implementou a "segunda chance"?
- Como trata o caso onde todas as páginas têm R=1?
- Como garante que o R-bit é setado em todo acesso?

**Não cole código aqui.** Explique a lógica em linguagem natural.

### 2.5 Tratamento de Page Fault

Explique como seu código distingue e trata os dois cenários:

**Cenário 1: Frame livre disponível**
- Como identifica que há frame livre?
- Quais passos executa para alocar a página?

**Cenário 2: Memória cheia (substituição)**
- Como identifica que a memória está cheia?
- Como decide qual algoritmo usar (FIFO vs Clock)?
- Quais passos executa para substituir uma página?

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
