/**
 * @file PluginContext.h
 * @brief Defines the PluginContext struct, which provides a narrow view of the engine's services
 * and type registry to plugins at load/unload time.
 */
#pragma once

namespace chai
{
    class ServiceLocator;
    class TypeRegistry;

    /**
     * @note Really only works for static plugins, since dynamic ones need to be able to query the
     * registry for services and types
     * @todo consider dynamic plugins
     */
    struct PluginContext {
        ServiceLocator& services;
        TypeRegistry& types;
    };
} // namespace chai