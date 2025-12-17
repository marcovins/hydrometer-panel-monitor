# 📧 Guia Rápido: Gmail OAuth2

## 🚀 Configuração em 5 Passos

### 1️⃣ Instalar libcurl

```bash
sudo apt-get update
sudo apt-get install libcurl4-openssl-dev
```

### 2️⃣ Criar projeto no Google Cloud

1. Acesse: https://console.cloud.google.com/
2. **Criar Projeto** > Nome: "SSMH Email" > Criar
3. Selecione o projeto criado

### 3️⃣ Ativar Gmail API

1. Menu > **APIs & Services** > **Enable APIs and Services**
2. Busque: "Gmail API"
3. Click **Enable**

### 4️⃣ Configurar OAuth2

**A) Tela de Consentimento:**
1. Menu > **APIs & Services** > **OAuth consent screen**
2. Escolha **External** > Create
3. Preencha:
   - App name: `SSMH Notifications`
   - User support email: `seu-email@gmail.com`
   - Developer contact: `seu-email@gmail.com`
4. Click **Save and Continue**
5. Em "Scopes", click **Add or Remove Scopes**
   - Adicione: `https://mail.google.com/`
6. **Save and Continue** > **Save and Continue** > **Back to Dashboard**

**B) Criar Credenciais:**
1. Menu > **APIs & Services** > **Credentials**
2. **Create Credentials** > **OAuth 2.0 Client ID**
3. Application type: **Desktop app**
4. Name: `SSMH Desktop Client`
5. **Create**
6. 📝 **Copie e guarde**:
   - **Client ID**: `123456...apps.googleusercontent.com`
   - **Client Secret**: `GOCSPX-abc...`

### 5️⃣ Obter Refresh Token

1. Acesse: https://developers.google.com/oauthplayground/
2. Click na **engrenagem ⚙️** (canto superior direito)
3. Marque: ☑️ **Use your own OAuth credentials**
4. Cole:
   - **OAuth Client ID**: (seu Client ID)
   - **OAuth Client secret**: (seu Client Secret)
5. Feche as configurações
6. Em **Step 1** (lado esquerdo):
   - Na caixa de busca, digite: `https://mail.google.com/`
   - Marque: ☑️ `https://mail.google.com/`
7. Click **Authorize APIs**
8. **Escolha sua conta Gmail**
9. Click **Continue** (pode aparecer aviso "Google hasn't verified")
10. Em **Step 2**, click **Exchange authorization code for tokens**
11. 📝 **Copie o Refresh token**

---

## ⚙️ Configurar no Projeto

### 1. Criar arquivo de configuração

```bash
cd /home/marcos/Faculdade/Padroes_de_Projeto/hydrometer-panel-monitor
cp config/email_config.example.hpp config/email_config.hpp
nano config/email_config.hpp
```

### 2. Editar com suas credenciais

```cpp
namespace EmailConfig {
    namespace Gmail {
        const std::string SMTP_SERVER = "smtp.gmail.com";
        const int SMTP_PORT = 587;
        
        // SUBSTITUA com seu email
        const std::string FROM_EMAIL = "seu-email@gmail.com";
        const std::string FROM_NAME = "Sistema SSMH - Alertas";
        
        // SUBSTITUA com suas credenciais (do passo 4 e 5)
        const std::string CLIENT_ID = "123456789-abc.apps.googleusercontent.com";
        const std::string CLIENT_SECRET = "GOCSPX-seu-secret-aqui";
        const std::string REFRESH_TOKEN = "1//seu-refresh-token-aqui";
    }
    
    const long TIMEOUT = 30;
    const bool VERBOSE = false;  // true para debug
    const bool VERIFY_SSL = true;
}
```

### 3. Habilitar no Makefile

Edite `Makefile` e **descomente** a linha:

```makefile
# Flags para configuração de email (opcional)
EMAIL_CONFIG_FLAG = -DEMAIL_CONFIG_EXISTS -I./config
```

Para:

```makefile
# Flags para configuração de email (opcional)
EMAIL_CONFIG_FLAG = -DEMAIL_CONFIG_EXISTS -I./config
```

### 4. Compilar e testar

```bash
make clean
make test-alertas
./test_alertas
```

---

## ✅ Verificação

Se tudo estiver correto, você verá:

```
[EMAIL] ✓ Configuração OAuth2 carregada com sucesso
[EMAIL] Remetente: seu-email@gmail.com
[EMAIL] 📧 Enviando email para destino@example.com...
[EMAIL] ✓ Email enviado com sucesso!
```

---

## 🐛 Problemas Comuns

### ❌ "curl/curl.h: No such file"
```bash
sudo apt-get install libcurl4-openssl-dev
```

### ❌ "undefined reference to curl_easy_init"
Verifique se `$(CURL_LIBS)` está no Makefile nos targets de alertas

### ❌ "Erro ao obter access token"
- Verifique Client ID e Client Secret
- Refresh token pode estar expirado - gere um novo

### ❌ "Email não configurado - usando modo simulação"
- Certifique-se de ter descomentado `EMAIL_CONFIG_FLAG` no Makefile
- Recompile: `make clean && make`

### 🔍 Debug detalhado

No `config/email_config.hpp`, defina:
```cpp
const bool VERBOSE = true;
```

Recompile e execute para ver logs completos do CURL.

---

## 📞 Teste Rápido

```cpp
#include "notificacao_email.hpp"

auto email = std::make_unique<NotificacaoEmail>();

if (email->isDisponivel()) {
    email->enviar(
        "Teste do sistema SSMH",
        "seu-email@gmail.com"  // envie para você mesmo
    );
}
```

---

## 🔒 Segurança

- ✅ `config/email_config.hpp` está no `.gitignore`
- ✅ NUNCA faça commit das credenciais
- ✅ Use `VERIFY_SSL = true` em produção
- ✅ Revise permissões no Google Cloud Console

---

**Pronto! 🎉** Seu sistema está configurado para enviar emails reais via Gmail OAuth2.

Para detalhes avançados, consulte: [docs/EMAIL_OAUTH2_SETUP.md](EMAIL_OAUTH2_SETUP.md)
