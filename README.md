PID-Falsificação (PPID Spoofing & Process Tree Evasion)


Técnica de OPSEC (Segurança Operacional) e Evasão de Defesas

MITRE ATT&CK: T1134.004 - Access Token Manipulation: Parent Process ID Spoofing

Do ponto de vista de um operador de Red Team ou de um analista de segurança, a manipulação do ID do processo pai (PPID Spoofing) é uma técnica fundamental para quebrar a árvore de processos e dificultar a detecção comportamental por EDRs e ferramentas de SIEM.

 Principais Objetivos e Vantagens
1. Quebra da Árvore de Processos (Process Tree Evasion)
A forma mais comum de um EDR (CrowdStrike, Defender for Endpoint, SentinelOne) ou analista de SOC detectar um malware é através de anomalias na árvore de processos.

Sem PPID Spoofing (Comportamento Malicioso Evidente):

Plaintext
WINWORD.EXE (Phishing)
└── cmd.exe ou powershell.exe (Alerta Crítico no SOC!)
O Word nunca deveria gerar um interpretador de comandos diretamente.

Com PPID Spoofing (Comportamento Legítimo):

Plaintext
explorer.exe (Interface Gráfica do Windows)
└── powershell.exe (Payload injetado sob o PID do Explorer)
Para a ferramenta de monitoramento, parece que o próprio usuário abriu o PowerShell, burlando a heurística baseada em parentesco.

2. Bypass de ASR (Attack Surface Reduction)
Ambientes corporativos modernos utilizam regras de ASR do Windows Defender para bloquear ações maliciosas comuns, como:

"Bloquear aplicativos do Office de criar processos filhos"

Se um documento Word tentar chamar CreateProcess de forma padrão, o Defender bloqueia imediatamente. Com o PPID Spoofing, o sistema operacional registra que o criador do processo é o explorer.exe (ou outro processo confiável), fazendo com que a regra de ASR seja ignorada.

3. Proteção do C2 (Beacon OPSEC)
Frameworks de Comando e Controle (Cobalt Strike, Mythic, Brute Ratel) utilizam processos sacrificiais (sacrificial processes) para executar tarefas pesadas ou arriscadas (ex: execução do Mimikatz, comandos de enumeração, execute-assembly).

Se você injetar um comando diretamente no processo principal e ele for detectado, o EDR encerrará o processo e você perderá o acesso à máquina.

Com PPID Spoofing: O comando roda sob um processo estável (como svchost.exe ou explorer.exe). Se o EDR interceptar o processo filho, ele abate apenas a tarefa secundária, mantendo o seu Beacon principal oculto e seguro.

4. Enganar a Análise Visual (Threat Hunting)
Analistas de SOC e Threat Hunters utilizam ferramentas como Process Hacker, Process Explorer ou analisam logs do Sysmon (Event ID 1) em busca de processos órfãos ou locais anômalos na árvore.

Fazer o seu payload rodar como filho de processos legítimos (Living off the Land):

explorer.exe: Para artefatos executados na sessão do usuário.

services.exe ou svchost.exe: Para tarefas executadas em nível de sistema.
Isso faz com que o seu artefato se misture ao ruído legítimo do sistema operacional.

  A Perspectiva do Blue Team (Limitações e Riscos)
Embora o PPID Spoofing engane o Gerenciador de Tarefas e regras básicas de correlação em SIEMs, EDRs modernos baseados em Kernel contam com mecanismos avançados para mitigar essa técnica:

ETW-Ti (Event Tracing for Windows - Threat Intelligence): O Kernel do Windows registra dois campos distintos: o Parent Process (que você falsificou para explorer.exe) e o Creator Process (que revela o processo real que invocou a API CreateProcess).

Nível de Integridade (Integrity Levels): Se um processo com privilégios normais (Medium Integrity) tentar definir um processo elevado (High/System Integrity) como pai, o Windows criará o processo, mas a inconsistência nos tokens de acesso será evidente em uma análise forense de memória.

Resumo para o Red Team: O PPID Spoofing é um pré-requisito essencial de OPSEC na criação de processos, mas deve ser combinado com técnicas complementares — como Block DLLs, Command Line Spoofing e Token Impersonation — para manter a invisibilidade diante de EDRs avançados.


*FUNCIONAMENTO DE UM EDR*

<img width="1600" height="820" alt="WhatsApp Image 2026-07-26 at 15 21 25" src="https://github.com/user-attachments/assets/c813c778-489f-4be2-9175-c3fecef23116" />
