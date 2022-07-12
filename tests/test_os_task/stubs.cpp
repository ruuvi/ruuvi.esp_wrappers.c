/**
 * @file stubs.cpp
 * @author TheSomeMan
 * @date 2020-12-08
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include <cstddef>
#include <cstdlib>
#include <cassert>
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "test_os_task.hpp"

using namespace std;

TestOsTask *g_pTestClass;

extern "C" {

void *
os_calloc(const size_t nmemb, const size_t size)
{
    return calloc(nmemb, size);
}

void
os_free_internal(void *ptr)
{
    free(ptr);
}

char *
pcTaskGetName(TaskHandle_t xTaskToQuery)
{
    assert(nullptr == xTaskToQuery);
    if (g_pTestClass->m_taskName == string(""))
    {
        return nullptr;
    }
    return const_cast<char *>(g_pTestClass->m_taskName.c_str());
}

UBaseType_t
uxTaskPriorityGet(const TaskHandle_t xTask)
{
    assert(nullptr == xTask);
    return g_pTestClass->m_createdTaskPriority;
}

TaskHandle_t
xTaskGetCurrentTaskHandle(void)
{
    return nullptr;
}

BaseType_t
xTaskCreate(
    TaskFunction_t               pxTaskCode,
    const char *const            pcName,
    const configSTACK_DEPTH_TYPE usStackDepth,
    void *const                  pvParameters,
    UBaseType_t                  uxPriority,
    TaskHandle_t *const          pxCreatedTask)
{
    if (nullptr == g_pTestClass->m_createdTaskHandle)
    {
        return pdFAIL;
    }
    *pxCreatedTask                  = g_pTestClass->m_createdTaskHandle;
    g_pTestClass->m_is_static       = false;
    g_pTestClass->m_createdTaskFunc = pxTaskCode;
    g_pTestClass->m_createdTaskName.assign(pcName);
    g_pTestClass->m_createdTaskStackDepth = usStackDepth;
    g_pTestClass->m_createdTaskParam      = pvParameters;
    g_pTestClass->m_createdTaskPriority   = uxPriority;
    return pdPASS;
}

TaskHandle_t
xTaskCreateStatic(
    TaskFunction_t      pxTaskCode,
    const char *const   pcName,
    const uint32_t      ulStackDepth,
    void *const         pvParameters,
    UBaseType_t         uxPriority,
    StackType_t *const  puxStackBuffer,
    StaticTask_t *const pxTaskBuffer)
{
    (void)puxStackBuffer;
    (void)pxTaskBuffer;
    if (nullptr == g_pTestClass->m_createdTaskHandle)
    {
        return nullptr;
    }
    g_pTestClass->m_is_static       = true;
    g_pTestClass->m_createdTaskFunc = pxTaskCode;
    g_pTestClass->m_createdTaskName.assign(pcName);
    g_pTestClass->m_createdTaskStackDepth = ulStackDepth;
    g_pTestClass->m_createdTaskParam      = pvParameters;
    g_pTestClass->m_createdTaskPriority   = uxPriority;
    return g_pTestClass->m_createdTaskHandle;
}

void
vTaskDelete(TaskHandle_t xTaskToDelete)
{
    (void)xTaskToDelete;
}

} // extern "C"
