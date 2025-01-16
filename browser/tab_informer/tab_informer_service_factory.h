#ifndef BRAVE_BROWSER_TAB_INFORMER_TAB_INFORMER_SERVICE_FACTORY_H_
#define BRAVE_BROWSER_TAB_INFORMER_TAB_INFORMER_SERVICE_FACTORY_H_

#include "base/no_destructor.h"
#include "components/keyed_service/content/browser_context_keyed_service_factory.h"

namespace content {
class BrowserContext;
}

namespace tab_informer {

class TabInformerService;

class TabInformerServiceFactory : public BrowserContextKeyedServiceFactory {
 public:
  static TabInformerServiceFactory* GetInstance();
  static TabInformerService* GetForBrowserContext(
      content::BrowserContext* context);

 protected:
  bool ServiceIsCreatedWithBrowserContext() const override;

 private:
  friend base::NoDestructor<TabInformerServiceFactory>;

  TabInformerServiceFactory();
  ~TabInformerServiceFactory() override;

  std::unique_ptr<KeyedService> BuildServiceInstanceForBrowserContext(
      content::BrowserContext* context) const override;
};

}  // namespace tab_informer

#endif  // BRAVE_BROWSER_TAB_INFORMER_TAB_INFORMER_SERVICE_FACTORY_H_
