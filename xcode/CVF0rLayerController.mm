/*  FreeJ
 *  (c) Copyright 2009 Andrea Guzzo <xant@dyne.org>
 *
 * This source code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License as published 
 * by the Free Software Foundation; either version 3 of the License,
 * or (at your option) any later version.
 *
 * This source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * Please refer to the GNU Public License for more details.
 *
 * You should have received a copy of the GNU Public License along with
 * this source code; if not, write to:
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#import <CVF0rLayerController.h>

@implementation CVF0rLayerController : CVLayerController

- (void)feedFrame:(void *)frame
{
    CVPixelBufferRef newPixelBuffer;
    //Context *ctx = (Context *)[freej getContext];
    [lock lock];
    CVReturn err = CVPixelBufferCreateWithBytes (
                                                 NULL,
                                                 layer->geo.w,
                                                 layer->geo.h,
                                                 k32ARGBPixelFormat,
                                                 frame,
                                                 layer->geo.w*4,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 &newPixelBuffer
                                                 ); 
    if (err == kCVReturnSuccess) {
        if (currentFrame)
            CVPixelBufferRelease(currentFrame);
        currentFrame = newPixelBuffer;
        newFrame = YES;
    }
    [lock unlock];
    [self renderPreview];
}

- (void)reset
{
    CVLayer *toDelete;
    if (layer) {
        [lock lock];
        if (currentFrame) {
            CVPixelBufferRelease(currentFrame);
            currentFrame = NULL;
        }
        toDelete = (CVLayer *)layer;
        layer = NULL;
        [lock unlock];
        toDelete->stop();
        delete toDelete;
    }
}

- (void)setLayer:(CVLayer *)lay
{
    if (layer) // ensure to remove/stop old genf0rlayer if we are setting a new one
        [self reset];
    [super setLayer:lay];
}

- (char *)name {
    if (layerView)
        return (char *)[[layerView toolTip] UTF8String];
    return (char *)"F0R";
}

@end
