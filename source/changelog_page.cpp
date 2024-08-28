#include "changelog_page.hpp"

#include "utils.hpp"

namespace i18n = brls::i18n;
using namespace i18n::literals;
ChangelogPage::ChangelogPage() : AppletFrame(true, true)
{
    this->setTitle("menus/changelog/changelog"_i18n);
    list = new brls::List();
    std::vector<std::string> verTitles;
    std::string change;
    std::vector<std::string> changes;

    verTitles.push_back("v1.0.1");
    changes.push_back("\uE016 Versão inicial.");

    verTitles.push_back("v1.0.2");
    changes.push_back("\uE016 Aba 'Ferramentas' incluída.\n\uE016 Várias opções incluídas na aba 'Ferramentas'.\n\uE016 Opção de atualizar o homebrew online incluída.\n\uE016 Código limpo.");

    verTitles.push_back("v1.0.3");
    changes.push_back(fmt::format("\uE016 Funções extras do AIO que não serão usados pelo {} foram removidas.\n\uE016 Melhorias nos textos.\n\uE016 Limpeza no código para remover dependência com o AIO.", APP_SHORT_NAME));

    verTitles.push_back("v1.1.0");
    changes.push_back("\uE016 Incluída uma função para baixar traduções.\n\uE016 Melhorias nos textos do homebrew.\n\uE016 Limpeza no código para remover dependência com o AIO.");

    verTitles.push_back("v1.1.1");
    changes.push_back("\uE016 Foi corrigido um problema de crash ao iniciar o homebrew sem internet.\n\uE016 Melhorias nos textos do homebrew.\n\uE016 Limpeza no código para remover dependência com o AIO.");

    verTitles.push_back("v1.1.2");
    changes.push_back("\uE016 Adicionada a opção de baixar o pacote GNX.\n\uE016 Melhorias nos textos do homebrew.\n\uE016 Nova imagem para o homebrew.");

    verTitles.push_back("v1.1.3");
    changes.push_back("\uE016 Aba 'Créditos' incluída.\n\uE016 Melhorias nos textos do homebrew.\n\uE016 Limpeza no código em geral.");

    verTitles.push_back("v1.1.4");
    changes.push_back("\uE016 Uma mensagem de aviso foi adicionada antes de downloads grandes.\n\uE016 Melhorias nos textos do homebrew.\n\uE016 Agora o homebrew usa o ícone em toda aba.\n\uE016 Limpeza no código para remover dependência com o AIO.");

    verTitles.push_back("v1.1.5");
    changes.push_back("\uE016 Corrigido um erro que fazia com que a aplicação não trocasse o nome do CFW sendo baixado.\n\uE016 Adicionada a função de deletar legenda instalada.\n\uE016 Melhorias nos textos do homebrew.");

    verTitles.push_back("v1.1.6");
    changes.push_back("\uE016 Aba 'Baixar modificações' incluída.\n\uE016 Incluída uma função que mostra com \u2605 as traduções e modificações instaladas.\n\uE016 Foi modificada a forma como o homebrew alerta sobre novas atualizações.\n\uE016 Limpeza no código em geral.\n\uE016 Melhorias nos textos do homebrew.");

    verTitles.push_back("v1.1.7");
    changes.push_back("\uE016 Corrigido um erro que fazia com que a aplicação não excluísse uma modificação instalada.\n\uE016 Incluída uma função para MOTD (message of the day - quadro de avisos).\n\uE016 Limpeza no código em geral.\n\uE016 Melhorias nos textos do homebrew.");
    
    verTitles.push_back("v1.1.8");
    changes.push_back("\uE016 Todo o código dos componentes auxiliares (forwarder e payload) foi reestruturado para uma melhor estabilidade.\n\uE016 A função que mostra com \u2605 as traduções instaladas foi ampliada para os demais itens (firmware, mods, etc).\n\uE016 Limpeza no código em geral.\n\uE016 Melhorias nos textos do homebrew.");

    verTitles.push_back("v2.0.0");
    changes.push_back("\uE016 O homebrew agora baixa conteúdo do Mega.nz.\n\uE016 O homebrew agora mostra o changelog dos pacotes baixados (quando disponível).\n\uE016 A posição dos botões nas telas de avisos e confirmação foi movida para mais em baixo agora.\n\uE016 O homebrew agora mostra o tamanho dos downloads em todos os lugares.\n\uE016 Correções internas na função de identificar conteúdos já instalados.\n\uE016 Foi incluída uma função de ajuda ao pressionar o botão \uE0E2.\n\uE016 Incluído um ítem em 'Ferramentas' para ver o último MOTD.\n\uE016 Limpeza no código em geral.\n\uE016 Melhorias nos textos do homebrew.");

    verTitles.push_back("v2.0.1");
    changes.push_back("\uE016 O homebrew agora detecta o tipo da emuNAND (SD ou partição) e mostra na aba 'Baixar Atmosphère'.\n\uE016 O homebrew agora alerta se você tem um tema instalado ANTES de baixar a atualização e te dá a opção de cancelar o processo.\n\uE016 O homebrew agora limpa 'payloads' antigos do HEKATE CTCAER na raiz do microSD.\n\uE016 Foi corrigido um problema que impedia o homebrew de mostrar o ícone quando um conteúdo estava instalado corretamente.\n\uE016 Limpeza no código em geral.\n\uE016 Melhorias nos textos do homebrew.");

    verTitles.push_back("v2.0.2");
    changes.push_back("\uE016 O homebrew agora alerta se você tem um tema instalado ANTES de baixar a atualização e te dá a opção de deleta-lo antes de prosseguir.\n\uE016 O homebrew agora mantém os principais arquivos de configuração do Tinfoil sempre que uma atualização é feita.\n\uE016 O homebrew agora te dá a opção de fazer uma instalação limpa mantendo arquivos e pastas importantes como emuNAND, pasta backup, saves (JKSV, Checkpoint e EdiZon), retroarch e suas ROMs, e arquivos importantes do Tinfoil.\n\uE016 Adicionada a opção de ver o changelog da nova atualização do homebrew antes de fazê-la.\n\uE016 Limpeza no código em geral.\n\uE016 Melhorias nos textos do homebrew.");

    verTitles.push_back("v2.0.3");
    changes.push_back("\uE016 Removido alguns estágios de 'Baixar Atmosphère' e 'Baixar firmware' para deixar tais opções mais diretas.\n\uE016 Adicionado temas do Tinfoil e o arquivo 'options.json' à lista de exclusão na atualização normal.\n\uE016 Resquícios de download de pacotes Atmosphère agora são apagados quando a opção 'Limpar arquivos' é usada em 'Ferramentas'.\n\uE016 Melhoria nos textos para esclarecer como a opção 'Instalação limpa' funciona.\n\uE016 Melhoria nos textos de 'Baixar Atmosphère' para esclarecer o que é opcional.");

    verTitles.push_back("v2.0.4");
    changes.push_back("\uE016 Melhoria nos textos para esclarecer um pouco mais sobre 'Instalação limpa'.\n\uE016 Limpeza no código em geral.\n\uE016 Melhorias nos textos do homebrew.");

	for (int i = verTitles.size() - 1; i >= 0; i--) {
        listItem = new brls::ListItem(verTitles[i]);
        change = changes[i];
        listItem->getClickEvent()->subscribe([change](brls::View* view) {
            util::showDialogBoxInfo(change);
        });
        list->addView(listItem);
    }
    this->setContentView(list);
}

void ChangelogPage::ShowChangelogContent(const std::string version, const std::string content)
{
    listItem = new brls::ListItem(version);
    listItem->getClickEvent()->subscribe([version, content](brls::View* view) {
        brls::AppletFrame* appView = new brls::AppletFrame(true, true);

        brls::PopupFrame::open(version, appView, "", "");
    });
    list->addView(listItem);
}