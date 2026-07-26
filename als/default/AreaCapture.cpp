/*
 * Copyright (C) 2021-2024 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "AreaCapture.h"

#include <gui/SurfaceComposerClient.h>
#include <gui/SyncScreenCaptureListener.h>
#include <ui/DisplayState.h>
#include <ui/PixelFormat.h>

using android::DisplayCaptureArgs;
using android::GraphicBuffer;
using android::IBinder;
using android::Rect;
using android::ScreenshotClient;
using android::sp;
using android::SurfaceComposerClient;
using android::SyncScreenCaptureListener;
using android::gui::ScreenCaptureResults;
using android::ui::PixelFormat;

namespace aidl {
namespace vendor {
namespace lineage {
namespace oplus_als {

AreaCapture::AreaCapture() {
    // Hardcoded values for the rectangle
    int left = 677;
    int top = 19;
    int right = 707;
    int bottom = 49;

    if (left < 0 || top < 0 || right <= left || bottom <= top) {
        LOG_ALWAYS_FATAL("Invalid screenshot grab area configuration");
        return;
    }

    ALOGI("Screenshot grab area: %d %d %d %d", left, top, right, bottom);
    m_screenshot_rect = Rect(left, top, right, bottom);
}

// See frameworks/base/services/core/jni/com_android_server_display_DisplayControl.cpp and
// frameworks/base/core/java/android/view/SurfaceControl.java
sp<IBinder> AreaCapture::getInternalDisplayToken() {
    const auto displayIds = SurfaceComposerClient::getPhysicalDisplayIds();
    if (displayIds.empty()) {
        ALOGE("No physical display IDs found");
        return nullptr;
    }
    return SurfaceComposerClient::getPhysicalDisplayToken(displayIds[0]);
}

ndk::ScopedAStatus AreaCapture::getAreaBrightness(AreaRgbCaptureResult* _aidl_return) {
    sp<IBinder> displayToken = getInternalDisplayToken();
    if (displayToken == nullptr) {
        ALOGE("Failed to get physical display token");
        return ndk::ScopedAStatus::fromServiceSpecificError(-1);
    }

    Rect cropRect = m_screenshot_rect;

    android::ui::DisplayState displayState;
    if (SurfaceComposerClient::getDisplayState(displayToken, &displayState) == ::android::NO_ERROR) {
        int w = 1080;
        int h = 2400;
        if (displayState.layerStackSpaceRect.width > 0 && displayState.layerStackSpaceRect.height > 0) {
            if (displayState.orientation == android::ui::Rotation::Rotation90 ||
                displayState.orientation == android::ui::Rotation::Rotation270) {
                w = displayState.layerStackSpaceRect.height;
                h = displayState.layerStackSpaceRect.width;
            } else {
                w = displayState.layerStackSpaceRect.width;
                h = displayState.layerStackSpaceRect.height;
            }
        }

        switch (displayState.orientation) {
            case android::ui::Rotation::Rotation90:
                cropRect = Rect(m_screenshot_rect.top,
                                w - m_screenshot_rect.right,
                                m_screenshot_rect.bottom,
                                w - m_screenshot_rect.left);
                break;
            case android::ui::Rotation::Rotation180:
                cropRect = Rect(w - m_screenshot_rect.right,
                                h - m_screenshot_rect.bottom,
                                w - m_screenshot_rect.left,
                                h - m_screenshot_rect.top);
                break;
            case android::ui::Rotation::Rotation270:
                cropRect = Rect(h - m_screenshot_rect.bottom,
                                m_screenshot_rect.left,
                                h - m_screenshot_rect.top,
                                m_screenshot_rect.right);
                break;
            case android::ui::Rotation::Rotation0:
            default:
                cropRect = m_screenshot_rect;
                break;
        }
    }

    DisplayCaptureArgs captureArgs;
    captureArgs.displayToken = displayToken;
    captureArgs.captureArgs.pixelFormat = static_cast<int32_t>(PixelFormat::RGBA_8888);
    captureArgs.captureArgs.sourceCrop.left = cropRect.left;
    captureArgs.captureArgs.sourceCrop.top = cropRect.top;
    captureArgs.captureArgs.sourceCrop.right = cropRect.right;
    captureArgs.captureArgs.sourceCrop.bottom = cropRect.bottom;
    captureArgs.width = cropRect.getWidth();
    captureArgs.height = cropRect.getHeight();
    captureArgs.captureArgs.secureLayerMode = ::android::gui::SecureLayerMode::Capture;

    sp<SyncScreenCaptureListener> captureListener = new SyncScreenCaptureListener();
    if (ScreenshotClient::captureDisplay(captureArgs, captureListener) != ::android::NO_ERROR) {
        ALOGE("Capture failed");
        return ndk::ScopedAStatus::fromServiceSpecificError(-1);
    }

    auto captureResults = captureListener->waitForResults();
    if (!captureResults.fenceResult.ok()) {
        ALOGE("Fence result error");
        return ndk::ScopedAStatus::fromServiceSpecificError(-1);
    }

    if (captureResults.buffer == nullptr) {
        ALOGE("Capture buffer is null");
        return ndk::ScopedAStatus::fromServiceSpecificError(-1);
    }

    uint8_t* out;
    captureResults.buffer->lock(GraphicBuffer::USAGE_SW_READ_OFTEN, reinterpret_cast<void**>(&out));

    auto resultWidth = captureResults.buffer->getWidth();
    auto resultHeight = captureResults.buffer->getHeight();
    auto stride = captureResults.buffer->getStride();

    if (out == nullptr || resultWidth <= 0 || resultHeight <= 0) {
        ALOGE("Invalid capture buffer dimensions or pointer");
        if (captureResults.buffer != nullptr) {
            captureResults.buffer->unlock();
        }
        return ndk::ScopedAStatus::fromServiceSpecificError(-1);
    }

    // we can sum this directly on linear light
    uint32_t rsum = 0, gsum = 0, bsum = 0;
    for (int y = 0; y < resultHeight; y++) {
        for (int x = 0; x < resultWidth; x++) {
            rsum += out[y * (stride * 4) + x * 4];
            gsum += out[y * (stride * 4) + x * 4 + 1];
            bsum += out[y * (stride * 4) + x * 4 + 2];
        }
    }

    float max = resultWidth * resultHeight;
    _aidl_return->r = rsum / max;
    _aidl_return->g = gsum / max;
    _aidl_return->b = bsum / max;

    captureResults.buffer->unlock();

    return ndk::ScopedAStatus::ok();
}

}  // namespace oplus_als
}  // namespace lineage
}  // namespace vendor
}  // namespace aidl
