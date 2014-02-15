//
//  BSShieldViewController.h
//  BlueShieldDemo
//
//  Created by yy on 13-4-14.
//  Copyright (c) 2013年 Linlinqi. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <CoreBluetooth/CoreBluetooth.h>

@class BlueShield;

@interface BSShieldViewController : UIViewController <UITextFieldDelegate>

@property (nonatomic, weak) BlueShield *shield;
@property (nonatomic, strong) CBPeripheral *peripheral;

@property (weak, nonatomic) IBOutlet UITextView *txtReceived;

- (void)processReceiveBuffer:(NSData*)data;

- (void) makeSendBuffer:(unsigned char*) buffer
          returnLength:(int*)length
          commandBuffer:(const unsigned char*)cBuffer
          commandLength:(int)cLength;

- (void) makeSyncTimeCommandBuffer:(unsigned char*) buffer
               returnLength:(int*)length;
- (void) makeSyncTimeBuffer:(unsigned char*) buffer
               returnLength:(int*)length;

- (void) makeDownloadCommandBuffer:(unsigned char*) buffer
               returnLength:(int*)length;
- (void) makeDownloadBuffer:(unsigned char*) buffer
               returnLength:(int*)length;

- (void) makeTaskCountCommandBuffer:(unsigned char*) buffer
                      returnLength:(int*)length;
- (void) makeTaskCountBuffer:(unsigned char*) buffer
               returnLength:(int*)length;
- (void) makeTasksCommandBuffer:(unsigned char*) buffer
                       returnLength:(int*)length
                       packetNo:(int)packetNo;
- (void) makeTasksBuffer:(unsigned char*) buffer
                returnLength:(int*)length
                packetNo:(int)packetNo;
- (void) makeTasksAckCommandBuffer:(unsigned char*) buffer
                      returnLength:(int*)length;
- (void) makeTasksAckBuffer:(unsigned char*) buffer
               returnLength:(int*)length;

- (void) sendTxBuffer:(unsigned char*) buffer
         sendLength:(int)length;

@end
