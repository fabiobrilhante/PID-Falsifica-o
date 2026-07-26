# PID-Falsifica-o
Quebrar a "Árvore de Processos" (Process Tree Evasion)

Do ponto de vista de um Red Team (ou de um atacante), essa técnica é conhecida como PPID Spoofing (Parent Process ID Spoofing).

Ela é uma técnica fundamental de OPSEC (Segurança Operacional) e Evasão de Defesas (T1134.004 no MITRE ATT&CK).

Abaixo estão os principais objetivos e o que conseguimos alcançar com ela durante um engajamento:

1. Quebrar a "Árvore de Processos" (Process Tree Evasion)
A forma mais comum de um EDR (CrowdStrike, Defender for Endpoint, SentinelOne) ou um analista de SOC detectar malware é analisando a anomalia na árvore de processos.

Sem PPID Spoofing (Comportamento Malicioso Evidente):

Um usuário abre um anexo de phishing (WINWORD.EXE).
A macro executa o seu malware e abre um terminal (WINWORD.EXE 
→
→ cmd.exe ou powershell.exe).
Resultado: Alerta crítico no SOC. O Word nunca deveria gerar um prompt de comando.
Com PPID Spoofing (Comportamento Legítimo):

WINWORD.EXE executa o seu payload.
O payload cria o powershell.exe, mas diz ao Windows que o pai é o explorer.exe (a interface gráfica do Windows).
Resultado: Na ferramenta do SOC, parece que o próprio usuário clicou no ícone do PowerShell. A heurística baseada em parentesco é burlada.

2. Bypass de ASR (Attack Surface Reduction)
Os ambientes corporativos modernos usam regras de ASR do Windows Defender para bloquear ações maliciosas comuns, como:

"Bloquear aplicativos do Office de criar processos filhos"

Se um documento Word tentar chamar CreateProcess normalmente, o Defender bloqueia. No entanto, com PPID Spoofing, como o sistema operacional registra que quem está criando o processo é o explorer.exe (ou outro processo confiável), a regra de ASR é ignorada.

3. Proteção do C2 (Beacon OPSEC)
Frameworks de C2 (como Cobalt Strike, Mythic, Brute Ratel) usam processos "descartáveis" (sacrificial processes) para rodar tarefas pesadas ou arriscadas (ex: rodar o Mimikatz, comandos de reconhecimento, execute-assembly).

Se você injetar um comando e ele for detectado, o EDR vai matar o processo e olhar quem é o pai.
Se o pai for o seu Beacon (seu acesso principal), você perde o acesso à máquina.
Com PPID Spoofing: Você faz o comando rodar embaixo do svchost.exe ou explorer.exe. Se o EDR pegar o processo filho, ele mata apenas a tarefa, e seu Beacon principal continua escondido e salvo.

4. Enganar a Análise Visual (Threat Hunting)
Quando um analista de SOC ou Threat Hunter está usando ferramentas como Process Hacker, Process Explorer ou analisando logs do Sysmon (Event ID 1), eles procuram por processos "órfãos" ou em locais estranhos da árvore.

Fazer o seu malware rodar como filho do:

explorer.exe (se for um malware executado na sessão do usuário).
services.exe (se for um serviço).
svchost.exe (para tarefas do sistema).
Faz com que seu artefato se "misture com o ruído" de fundo do sistema operacional (Living off the Land).


*******************************************************   O Lado do "Blue Team" (O que o Red Team precisa saber que dá errado)  ***********************************************************************
Embora o PPID Spoofing engane o Gerenciador de Tarefas e regras básicas de SIEM, EDRs modernos de nível Kernel não são 100% enganados por esse código básico porque:

ETW-Ti (Event Tracing for Windows - Threat Intelligence): O Windows kernel relata dois campos: o Parent Process (que você falsificou para explorer.exe) e o Creator Process (que revela quem realmente chamou a API CreateProcess).
Nível de Integridade: Se você (com privilégios normais - Medium Integrity) tentar colocar um processo elevado (High/System Integrity) como pai, o Windows deixará criar o processo, mas os níveis de integridade e os tokens podem parecer inconsistentes para uma análise profunda de memória.
Resumo para o Red Team: PPID Spoofing é um pré-requisito obrigatório de OPSEC na criação de processos hoje em dia, mas deve ser combinado com técnicas como Block DLLs, Command Line Spoofing e PPID com Token Impersonation para ser invisível a EDRs avançados.


*FUNCIONAMENTO DE UM EDR*

<img width="1600" height="820" alt="WhatsApp Image 2026-07-26 at 15 21 25" src="https://github.com/user-attachments/assets/c813c778-489f-4be2-9175-c3fecef23116" />
