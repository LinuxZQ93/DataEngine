#include "IProtocol.h"

#include "Print/Print.h"
#include "ProtocolManager.h"

namespace engine {

IProtocol::IProtocol()
{
    m_pDataEngine = base::CComponentManager::instance()->getComponent<IDataEngine>("DataEngine");
}

IProtocol::~IProtocol()
{}

IProtocol::IProtocolFactory::IProtocolFactory(const std::string &name)
    : m_protocol(nullptr)
{
}

IProtocol::IProtocolFactory::~IProtocolFactory()
{}

bool IProtocol::IProtocolFactory::registerProtocol(const std::string &taskName)
{
    if (nullptr == m_protocol)
    {
        warnf("protocol is null, register failed\n");
        return false;
    }
    return CProtocolManager::instance()->registerProtocol(taskName, m_protocol);
}

}
