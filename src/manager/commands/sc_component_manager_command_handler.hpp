/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "commandsModule/keynodes/commands_keynodes.hpp"
#include "commandsModule/agents/sc_component_manager_agent_search.hpp"

#include <utility>

#include "sc-agents-common/utils/CommonUtils.hpp"

#include "sc_component_manager_handler.hpp"
#include "sc_component_manager_command.hpp"
#include "command_init/sc_component_manager_command_init.hpp"
#include "command_search/sc_component_manager_command_search.hpp"
#include "command_install/sc_component_manager_command_install.hpp"

class ScComponentManagerCommandHandler : public ScComponentManagerHandler
{
public:
  explicit ScComponentManagerCommandHandler(std::map<ScAddr, std::string, ScAddrLessFunc> componentsPath)
    : m_componentsPath(std::move(componentsPath))
  {
    m_context = new ScMemoryContext();
    std::string const & specificationPath =
        m_componentsPath.at(keynodes::ScComponentManagerKeynodes::concept_reusable_kb_component);
    m_actions = {
        {"init", new ScComponentManagerCommandInit(specificationPath)},
        {"search", new ScComponentManagerCommandSearch()},
        {"install", new ScComponentManagerCommandInstall(m_componentsPath)}};
  }

  bool Handle(std::string const & commandType, CommandParameters const & commandParameters) override
  {
    bool executionResult;
    m_commandParameters = commandParameters;
    auto const & it = m_actions.find(commandType);
    if (it != m_actions.end())
    {
      ScComponentManagerCommand * commander = it->second;
      SC_LOG_DEBUG("ScComponentManagerCommandHandler: execute " + it->first + " command");
//      if (it->first == "search")
//      {
//        ScAddr actionAddr = CreateActionAddrWithClass(m_context, "action_components_search");
          // there's an error
//        ScComponentManagerSearchAgent::TransformToScStruct(*m_context, actionAddr, commandParameters);
//        m_context->CreateEdge(
//            ScType::EdgeAccessConstPosPerm, scAgentsCommon::CoreKeynodes::question_initiated, actionAddr);
//        executionResult = true;
//      }
        executionResult = commander->Execute(m_context, m_commandParameters);
    }
    else
    {
      SC_THROW_EXCEPTION(utils::ExceptionParseError, "Unsupported command type \"" + commandType + "\"");
    }

    return executionResult;
  }

//  ScAddr CreateActionAddrWithClass(ScMemoryContext * context, std::string const & actionClassName)
//  {
//    ScAddr actionAddr = context->CreateNode(ScType::NodeConst);
//    ScAddr actionClass = context->HelperFindBySystemIdtf(actionClassName);
//    context->CreateEdge(ScType::EdgeAccessConstPosPerm, actionClass, actionAddr);
//    return actionAddr;
//  }

  ~ScComponentManagerCommandHandler() override
  {
    m_context->Destroy();
    delete m_context;

    for (auto const & it : m_actions)
      delete it.second;

    m_actions.clear();
  }

protected:
  ScMemoryContext * m_context{};
  CommandParameters m_commandParameters;
  std::map<ScAddr, std::string, ScAddrLessFunc> m_componentsPath;
  std::map<std::string, ScComponentManagerCommand *> m_actions;
};
