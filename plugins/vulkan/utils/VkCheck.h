/**
 * @file VkCheck.h
 * @brief Helpers to log and abort on vulkan failures
 */
#pragma once
#include <vulkan/vulkan.h>
#include <Log.h>

namespace chai
{
    const char* vkResultString(VkResult result);
} // namespace chai


#define VK_CHECK(expr)                                                                             \
{                                                                                                  \
const VkResult _vk_r = (expr);                                                                     \
if (_vk_r != VK_SUCCESS) {                                                                         \
    CHAI_LOG_CRITICAL("VK_CHECK failed: {} -> {}", #expr, ::chai::vkResultString(_vk_r));          \
    std::abort();                                                                                  \
}                                                                                                  \
}

