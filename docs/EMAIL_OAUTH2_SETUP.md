# 📧 Guia de Configuração de Email com OAuth2

## Visão Geral

O sistema de notificação por email foi implementado com suporte real a envio via SMTP usando **libcurl** com autenticação **OAuth2**. Suporta Gmail, Outlook/Office365 e servidores SMTP personalizados.

## 🎯 Funcionalidades

- ✅ **OAuth2 Authentication** (Gmail e Outlook)
- ✅ **Autenticação tradicional** (username/password)
- ✅ **SMTP/STARTTLS** (porta 587)
- ✅ **Refresh token automático**
- ✅ **Fallback para modo simulação** se não configurado
- ✅ **Validação de email**
- ✅ **UTF-8 support** com Base64 encoding
- ✅ **Timeout configurável**
- ✅ **Modo verbose para debug**

---

## 📋 Pré-requisitos

### 1. Instalar libcurl

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install libcurl4-openssl-dev
```

**Fedora/RHEL:**
```bash
sudo dnf install libcurl-devel
```

**macOS:**
```bash
brew install curl
```

### 2. Verificar instalação
```bash
curl-config --version
curl-config --libs
```

---

## 🔧 Configuração

### Passo 1: Criar arquivo de configuração

```bash
cd /home/marcos/Faculdade/Padroes_de_Projeto/hydrometer-panel-monitor
cp config/email_config.example.hpp config/email_config.hpp
```

### Passo 2: Obter credenciais OAuth2

#### Para Gmail

1. Acesse [Google Cloud Console](https://console.cloud.google.com/)
2. Crie um projeto ou use um existente
3. Ative a **Gmail API**:
   - APIs & Services > Enable APIs and Services
   - Procure "Gmail API" e ative

4. Configure a tela de consentimento:
   - APIs & Services > OAuth consent screen
   - Escolha "External" (para testes)
   - Preencha os campos obrigatórios
   - Adicione escopes: `https://mail.google.com/`

5. Crie credenciais OAuth 2.0:
   - APIs & Services > Credentials
   - Create Credentials > OAuth 2.0 Client ID
   - Application type: Desktop app
   - Copie o **Client ID** e **Client Secret**

6. Obtenha o Refresh Token usando [OAuth2 Playground](https://developers.google.com/oauthplayground/):
   - Click na engrenagem (⚙️) no canto superior direito
   - Marque "Use your own OAuth credentials"
   - Cole seu Client ID e Client Secret
   - No Step 1, selecione: `https://mail.google.com/`
   - Click "Authorize APIs"
   - Faça login com sua conta Gmail
   - No Step 2, click "Exchange authorization code for tokens"
   - Copie o **Refresh Token**

#### Para Outlook/Office 365

1. Acesse [Azure Portal](https://portal.azure.com/)
2. Azure Active Directory > App registrations > New registration
3. Configure:
   - Name: "SSMH Email Notifications"
   - Supported account types: Accounts in any organizational directory and personal Microsoft accounts
   - Redirect URI: `https://login.microsoftonline.com/common/oauth2/nativeclient`

4. Copie o **Application (client) ID** e **Directory (tenant) ID**

5. Certificates & secrets > New client secret
   - Copie o **Client Secret** (value, não o ID)

6. API permissions:
   - Add permission > Microsoft Graph > Delegated permissions
   - Adicione: `SMTP.Send`, `offline_access`
   - Grant admin consent

7. Obter refresh token (use uma ferramenta como Postman ou script):
   ```bash
   # POST para obter authorization code
   https://login.microsoftonline.com/common/oauth2/v2.0/authorize?
     client_id=YOUR_CLIENT_ID&
     response_type=code&
     redirect_uri=https://login.microsoftonline.com/common/oauth2/nativeclient&
     scope=https://outlook.office365.com/SMTP.Send%20offline_access
   
   # POST para trocar code por tokens
   curl -X POST https://login.microsoftonline.com/common/oauth2/v2.0/token \
     -d "client_id=YOUR_CLIENT_ID" \
     -d "client_secret=YOUR_CLIENT_SECRET" \
     -d "code=AUTHORIZATION_CODE" \
     -d "grant_type=authorization_code" \
     -d "redirect_uri=https://login.microsoftonline.com/common/oauth2/nativeclient"
   ```

### Passo 3: Configurar email_config.hpp

Edite `config/email_config.hpp`:

```cpp
namespace EmailConfig {
    // Escolha: "GMAIL", "OUTLOOK", ou "CUSTOM"
    const std::string PROVIDER = "GMAIL";
    
    namespace Gmail {
        const std::string SMTP_SERVER = "smtp.gmail.com";
        const int SMTP_PORT = 587;
        const std::string FROM_EMAIL = "seu-email@gmail.com";
        const std::string FROM_NAME = "Sistema SSMH - Alertas";
        
        const std::string CLIENT_ID = "123456789.apps.googleusercontent.com";
        const std::string CLIENT_SECRET = "seu-client-secret";
        const std::string REFRESH_TOKEN = "seu-refresh-token";
    }
    
    // Configurações opcionais
    const long TIMEOUT = 30;        // segundos
    const bool VERBOSE = false;     // true para debug
    const bool VERIFY_SSL = true;   // true em produção
}
```

### Passo 4: Habilitar no Makefile

Edite o `Makefile` e descomente:

```makefile
# Flags para configuração de email (opcional)
EMAIL_CONFIG_FLAG = -DEMAIL_CONFIG_EXISTS -I./config
```

### Passo 5: Compilar

```bash
make clean
make test-alertas
```

---

## 🚀 Como Usar

### Modo Programático

```cpp
#include "notificacao_email.hpp"

// A configuração é carregada automaticamente
auto notificacaoEmail = std::make_unique<NotificacaoEmail>();

// Verificar se está configurado
if (notificacaoEmail->isDisponivel()) {
    // Enviar email
    bool sucesso = notificacaoEmail->enviar(
        "Alerta: Consumo acima do normal (150L)",
        "usuario@exemplo.com"
    );
    
    if (sucesso) {
        std::cout << "Email enviado!" << std::endl;
    }
} else {
    std::cout << "Email não configurado (modo simulação)" << std::endl;
}
```

### Configuração Manual (Alternativa)

```cpp
auto notificacao = std::make_unique<NotificacaoEmail>();

// Configurar OAuth2
notificacao->configurarOAuth2(
    "client-id",
    "client-secret",
    "refresh-token"
);

// OU configurar autenticação simples
notificacao->configurarAutenticacaoSimples(
    "username@example.com",
    "password"
);

notificacao->enviar("Mensagem", "destino@example.com");
```

---

## 🧪 Testando

### Teste Básico

```bash
make test-alertas
```

Este comando irá:
1. Carregar configurações de `config/email_config.hpp`
2. Se configurado: enviar emails reais
3. Se não configurado: simular envio

### Teste com Verbose (Debug)

No `email_config.hpp`, defina:
```cpp
const bool VERBOSE = true;
```

Recompile e execute para ver logs detalhados do CURL.

### Verificar se libcurl está linkada

```bash
ldd test_alertas | grep curl
```

Deve mostrar algo como:
```
libcurl.so.4 => /usr/lib/x86_64-linux-gnu/libcurl.so.4
```

---

## ⚠️ Troubleshooting

### Erro: "curl/curl.h: No such file or directory"

**Solução:**
```bash
sudo apt-get install libcurl4-openssl-dev
```

### Erro: "undefined reference to curl_easy_init"

**Solução:** Certifique-se que o Makefile tem:
```makefile
CURL_LIBS = -lcurl
```

E que os targets usam `$(CURL_LIBS)`

### Erro: "Access token inválido"

**Causas possíveis:**
1. Client ID/Secret incorretos
2. Refresh token expirado/inválido
3. Escopes OAuth2 incorretos

**Solução:**
- Gere um novo refresh token
- Verifique os scopes (Gmail: `https://mail.google.com/`)
- Habilite modo VERBOSE para ver detalhes

### Erro: SSL certificate problem

**Solução temporária (apenas para testes):**
```cpp
const bool VERIFY_SSL = false; // NÃO use em produção!
```

**Solução correta:**
```bash
sudo apt-get install ca-certificates
sudo update-ca-certificates
```

### Email não enviando mas sem erros

1. Verifique se OAuth2 está habilitado na conta:
   - Gmail: Verificar "Less secure app access"
   - Outlook: Verificar permissões da app

2. Teste conexão SMTP:
```bash
curl -v --ssl-reqd \
  --url 'smtp://smtp.gmail.com:587' \
  --mail-from 'seu-email@gmail.com' \
  --mail-rcpt 'destino@example.com'
```

---

## 📊 Fluxo de Autenticação OAuth2

```
┌─────────────────────────────────────────────────────────────┐
│ 1. Sistema verifica se tem access_token válido              │
│    - Se válido: usa direto                                   │
│    - Se inválido/expirado: vai para passo 2                 │
└─────────────────────────────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│ 2. Obtém novo access_token usando refresh_token             │
│    POST https://oauth2.googleapis.com/token                  │
│    - client_id, client_secret, refresh_token                │
└─────────────────────────────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│ 3. Configura CURL com XOAUTH2                                │
│    - CURLOPT_XOAUTH2_BEARER = access_token                   │
└─────────────────────────────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│ 4. Envia email via SMTP                                      │
│    - smtp://smtp.gmail.com:587                               │
│    - STARTTLS automático                                     │
└─────────────────────────────────────────────────────────────┘
```

---

## 🔒 Segurança

### ⚠️ IMPORTANTE

1. **NUNCA** faça commit do `config/email_config.hpp`
   - Já está no `.gitignore`
   - Contém credenciais sensíveis

2. **Use variáveis de ambiente** em produção:
   ```cpp
   const std::string CLIENT_ID = std::getenv("EMAIL_CLIENT_ID") 
       ? std::getenv("EMAIL_CLIENT_ID") : "";
   ```

3. **Rotacione secrets** periodicamente

4. **Use VERIFY_SSL = true** em produção

5. **Limite permissões OAuth2** ao mínimo necessário

---

## 📚 Referências

- [libcurl Documentation](https://curl.se/libcurl/)
- [Gmail SMTP OAuth2](https://developers.google.com/gmail/imap/xoauth2-protocol)
- [Microsoft Graph Mail Send](https://docs.microsoft.com/en-us/graph/api/user-sendmail)
- [OAuth2 RFC](https://tools.ietf.org/html/rfc6749)

---

## ✅ Checklist de Implementação

- [x] Instalar libcurl
- [ ] Criar conta Google Cloud / Azure Portal
- [ ] Obter Client ID e Client Secret
- [ ] Obter Refresh Token
- [ ] Criar config/email_config.hpp
- [ ] Configurar credenciais
- [ ] Descomentar EMAIL_CONFIG_FLAG no Makefile
- [ ] Compilar: `make clean && make test-alertas`
- [ ] Testar envio de email
- [ ] (Opcional) Habilitar VERBOSE para debug
- [ ] Desabilitar VERBOSE em produção

---

**Autor:** Marcos Belo  
**Data:** 16/12/2025  
**Versão:** 1.0

**Suporte:** Para problemas, verifique os logs com `VERBOSE = true` e consulte a seção de Troubleshooting.
