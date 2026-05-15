# Algoritmos-e-Programacao

Aulas da faculdade de Engenharia de Controle e Automação
Período: 2023/1 - 2023/2

## 📚 Conteúdo

* Estrutura de Dados I
* Algoritmos e Programação
* Exercícios e exemplos desenvolvidos em aula

---

# 🛠️ Comandos Git

## Configuração inicial

```bash
git config --global --list
# Exibe as informações do usuário configuradas no Git

git config --global user.name "<nome-do-usuario>"
# Configura o nome do usuário

git config --global user.email "<email-usuario>"
# Configura o e-mail do usuário
```

---

## Inicialização e status do repositório

```bash
git init
# Inicializa um novo repositório Git ou reinicia um existente

git status
# Mostra o status do repositório
# Indica arquivos modificados, adicionados ou pendentes de commit

git remote -v
# Verifica para onde o repositório está enviando os commits
```

---

## Adicionando arquivos e commits

```bash
git add <nome-do-arquivo>
# Adiciona um arquivo ao stage
# Para adicionar todos os arquivos, utilize:
git add .

git rm --cached <nome-do-arquivo>
# Remove o arquivo do stage, retornando para edição

git commit -m "mensagem"
# Realiza um commit com uma mensagem descritiva
```

---

## Histórico de commits

```bash
git log
# Exibe o histórico completo de commits

git log --oneline
# Exibe o histórico resumido de commits
```

---

## Navegação e recuperação de versões

```bash
git checkout <id-do-commit>
# Volta para um commit específico
# Para retornar ao estado atual, utilize o nome da branch

git revert <id-do-commit>
# Desfaz um commit específico criando um novo commit de reversão

git reset <id-do-commit>
# Remove commits posteriores, mantendo as alterações nos arquivos

git reset <id-do-commit> --hard
# Remove commits posteriores e apaga todas as alterações realizadas
```

---

## Branches

```bash
git branch <nome>
# Cria uma nova branch

git branch -d <nome>
# Remove uma branch vazia

git branch -D <nome>
# Remove uma branch não fundida

git merge <branch>
# Faz a fusão da branch informada com a branch atual
```

---

## Repositórios remotos

```bash
git push <endereco-repositorio-git.git>
# Envia o repositório local para o GitHub

git remote add origin <endereco-repositorio-git.git>
# Vincula o repositório local ao remoto

git pull origin <branch>
# Atualiza o repositório local com os commits remotos

git clone <endereco-repositorio-git.git> </destino>
# Clona um repositório do GitHub
```

---

# 🔐 Gerando chave SSH

No terminal:

```bash
ssh-keygen -t rsa
```

Após executar o comando:

1. Pressione `Enter` para confirmar o local padrão do arquivo
2. Informe uma senha (opcional)
3. Localize o arquivo:

```bash
id_rsa.pub
```

4. Copie o conteúdo da chave pública e adicione no GitHub em:

`Settings > SSH and GPG keys > New SSH key`
