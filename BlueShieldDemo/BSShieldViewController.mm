//
//  BSShieldViewController.m
//  BlueShieldDemo
//
//  Created by yy on 13-4-14.
//  Copyright (c) 2013年 Linlinqi. All rights reserved.
//

#import "BSShieldViewController.h"
#import "BlueShield.h"
#import "BSDefines.h"
#include "common/BCDUtilities.h"
#include "common/Array.h"
#include "common/ByteArray.h"
#include "common/MemoryStream.h"
#include "common/Crc16Utilities.h"

using namespace Common;

const int Header = 0xEE;
const int Tail = 0xEF;

static int _frameId = 0;

struct PlanInfo{
    byte No;            // plan no.
    ushort Interval;    // unit: min
};

static PlanInfo _plans[99];
static int _planCount = 0;

static bool _retrivedTasks = false;
static byte _tasksBuffer[512];
static int _tasksBufferCount = 0;
static NSObject *_tasksLocker = [[NSObject alloc] init];

struct TaskInfo{
    byte PlanNo;        // plan on.
    time_t StartTime;   // the task start time, unit: sec.
    byte TimeCount;
    time_t Times[99];   // the max value is 99, cz TimeCount is BCD value.
};

static TaskInfo _tasks[99];
static int _taskCount = 0;

@interface BSShieldViewController ()

@end

@implementation BSShieldViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view.
    
    // test start
    _planCount = 7;
    _plans[0].No = 1;
    _plans[0].Interval = 60;
    _plans[1].No = 2;
    _plans[1].Interval = 240;
    _plans[2].No = 3;
    _plans[2].Interval = 320;
    for (byte j=3; j<_planCount; j++) {
        _plans[j].No = j + 1;
        _plans[j].Interval = 0420 + j * 10;
    }
    // test end
    
    [_shield connectPeripheral:_peripheral];
    
    [_shield didDiscoverCharacteristicsBlock:^(id response, NSError *error) {
        double delayInSeconds = 3.0;
        dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delayInSeconds * NSEC_PER_SEC));
        dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
            [_shield notification:[CBUUID UUIDWithString:BS_SERIAL_SERVICE_UUID]
               characteristicUUID:[CBUUID UUIDWithString:BS_SERIAL_RX_UUID]
                                p:_peripheral
                                on:YES];
            
            [_shield didUpdateValueBlock:^(NSData *data, NSError *error) {
                byte* buffer = (byte*)[data bytes];
                int length = [data length];
                ByteArray array(buffer, length);
#if DEBUG
                NSDateFormatter *DateFormatter=[[NSDateFormatter alloc] init];
                [DateFormatter setDateFormat:@"yyyy-MM-dd hh:mm:ss.SSS"];
                NSString* timeStr = [DateFormatter stringFromDate:[NSDate date]];
                printf("time: %s, recv buffer: %s\n", [timeStr UTF8String], array.toString().data());
#endif
                NSString *recv = [[NSString alloc] initWithUTF8String:array.toString().data()];
                if(recv != nil){
                    _rxLabel.text = [NSString stringWithFormat:@"%@%@\n", _rxLabel.text, recv];
                }
                
                [self processReceiveBuffer:data];                
            }];
        });
    }];
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)processReceiveBuffer:(NSData*)data{
    byte* buffer = (byte*)[data bytes];
    int length = [data length];
    
    if(length > 0){
        if (_retrivedTasks) {
            @synchronized(_tasksLocker){
                memcpy(&_tasksBuffer[_tasksBufferCount], buffer, length);
                _tasksBufferCount += length;
                
                if (_tasksBufferCount >= 4) {
                    MemoryStream ms(_tasksBuffer + 2, length);
                    ushort len = ms.readBCDUInt16();
                    if(_tasksBufferCount >= len + 4 &&
                       buffer[length-1] == Tail){
                        _retrivedTasks = false;
#if DEBUG
                        ByteArray array(_tasksBuffer, _tasksBufferCount);
                        NSDateFormatter *DateFormatter=[[NSDateFormatter alloc] init];
                        [DateFormatter setDateFormat:@"yyyy-MM-dd hh:mm:ss.SSS"];
                        NSString* timeStr = [DateFormatter stringFromDate:[NSDate date]];
                        printf("time: %s, recv full buffer: %s\n", [timeStr UTF8String], array.toString().data());
#endif
                        // process 0x24 command.
                        MemoryStream stream(_tasksBuffer + 5, _tasksBufferCount);
                        _taskCount = stream.readBCDByte();
                        for(int i=0;i<_taskCount;i++){
                            _tasks[i].PlanNo = stream.readBCDByte();
                            _tasks[i].StartTime = stream.readBCDDateTime();
                            _tasks[i].TimeCount = stream.readBCDByte();
                            for (int j=0; j<_tasks[i].TimeCount; j++) {
                                _tasks[i].Times[j] = stream.readBCDTime();
                            }
                        }
                        
                        // send ack.
                        byte buffer[512];
                        memset(buffer, 0, sizeof(buffer));
                        int length = 0;
                        [self makeTasksAckBuffer:buffer returnLength:&length];
                        
                        [self sendTxBuffer:buffer sendLength:length];
                        
                        _rxLabel.text = [NSString stringWithFormat:@"%@Get the tasks successfully.", _rxLabel.text];
                    }
                }
            }
        }
        else{
            MemoryStream stream(buffer, length);
            if (length > 8)
            {
                byte header = stream.readByte();
                byte tail = buffer[length-1];
                if(header == Header && tail == Tail){
                    ushort expected = Crc16Utilities::CheckByBit(buffer, 0, length-3); // remove cr16 & tail
                    byte crcHigh = buffer[length-3];
                    byte crcLow = buffer[length-2];
                    ushort actual = ((crcHigh << 8) & 0xFF00) + crcLow;
                    if(expected == actual){
                        stream.readByte();               // skip frameId
                        stream.readInt16();              // skip length
                        byte command = stream.readByte();
                        if(command == 0x21){             // sync time.
                            byte status = stream.readByte();
                            _rxLabel.text = [NSString stringWithFormat:@"%@Sync time successfully. status is %d\n", _rxLabel.text, status];
                        }
                        else if(command == 0x22){        // download.
                            byte status = stream.readByte();
                            _rxLabel.text = [NSString stringWithFormat:@"%@Download the plans successfully. status is %d\n", _rxLabel.text, status];
                        }
                        else if(command == 0x23){        // retrived the packet count.
                            byte status = stream.readByte();
                            if(status == 0){             // successfully
                                byte packetCount = stream.readByte();
                                for (byte i=0; i<packetCount; i++) {
                                    byte buffer[512];
                                    memset(buffer, 0, sizeof(buffer));
                                    int length = 0;
                                    [self makeTasksBuffer:buffer returnLength:&length packetNo:i];

                                    [self sendTxBuffer:buffer sendLength:length];
                                }
                            }
                        }
                        else if(command == 0x24){        // retrived the packet.
                            @synchronized(_tasksLocker){
                                _retrivedTasks = true;
                                _tasksBufferCount = 0;
                                
                                memcpy(&_tasksBuffer[_tasksBufferCount], buffer, length);
                                _tasksBufferCount += length;
                            }
                        }
                    }
                }
            }
        }
    }
}

#pragma mark - custom method

- (void)sendTx {
    NSData *data = [_sendText.text dataUsingEncoding:NSUTF8StringEncoding];
    [_shield writeValue:[CBUUID UUIDWithString:BS_SERIAL_SERVICE_UUID]
     characteristicUUID:[CBUUID UUIDWithString:BS_SERIAL_TX_UUID]
                      p:_peripheral
                   data:data];
}

#pragma mark - UITextFieldDelegate

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    [textField resignFirstResponder];
    return YES;
}

- (void)textFieldDidEndEditing:(UITextField *)textField {
    [textField resignFirstResponder];
    [self sendTx];
}

- (void) makeSendBuffer:(unsigned char*) buffer
          returnLength:(int*)length
          commandBuffer:(const unsigned char*)cBuffer
          commandLength:(int)cLength{
    MemoryStream stream;
    
    stream.writeByte(Header);         // header
    stream.writeByte(_frameId++);   // frame
    if(_frameId > 0x3f)
        _frameId = 0;
    stream.writeUInt16(0);          // length
    int position = stream.position();
    
    stream.write(cBuffer, 0, cLength);
    
    int crcPosition = stream.position();
    ushort len = crcPosition - position + 3;
    stream.seek(position - 2);
    stream.writeBCDUInt16(len);
    stream.seek(crcPosition);
    
    stream.copyTo(buffer);
    ushort crc16 = Crc16Utilities::CheckByBit(buffer, 0, stream.length());
    stream.writeUInt16((crc16));
    stream.writeByte(Tail);
    
#if DEBUG
    NSDateFormatter *DateFormatter=[[NSDateFormatter alloc] init];
    [DateFormatter setDateFormat:@"yyyy-MM-dd hh:mm:ss.SSS"];
    NSString* timeStr = [DateFormatter stringFromDate:[NSDate date]];
    printf("time: %s, send buffer: %s\n", [timeStr UTF8String], stream.buffer()->toString().data());
#endif
    
    stream.copyTo(buffer);
    *length = stream.length();
}

- (void) makeSyncTimeCommandBuffer:(unsigned char*) buffer
               returnLength:(int*)length{
    MemoryStream stream;
    
    stream.writeByte(0x21);         // command
    stream.writeBCDCurrentTime();
    
    stream.copyTo(buffer);
    *length = stream.length();
}
- (void) makeSyncTimeBuffer:(unsigned char*) buffer
               returnLength:(int*)length{
    byte cBuffer[32];
    memset(cBuffer, 0, sizeof(cBuffer));
    int cLength = 0;
    [self makeSyncTimeCommandBuffer:cBuffer returnLength:&cLength];
    
    [self makeSendBuffer:buffer returnLength:length commandBuffer:cBuffer commandLength:cLength];
}

- (void) makeDownloadCommandBuffer:(unsigned char*) buffer
               returnLength:(int*)length{
    MemoryStream stream;
    
    stream.writeByte(0x22);         // command
    stream.writeByte(BCDUtilities::ByteToBCD(_planCount));
    for(int i=0;i<_planCount;i++){
        stream.writeBCDByte(_plans[i].No);
        byte hour = _plans[i].Interval / 60;
        byte minute = _plans[i].Interval % 60;
        stream.writeBCDByte(hour);
        stream.writeBCDByte(minute);
    }
    
    stream.copyTo(buffer);
    *length = stream.length();
}
- (void) makeDownloadBuffer:(unsigned char*) buffer
               returnLength:(int*)length{
    byte cBuffer[32];
    memset(cBuffer, 0, sizeof(cBuffer));
    int cLength = 0;
    [self makeDownloadCommandBuffer:cBuffer returnLength:&cLength];
    
    [self makeSendBuffer:buffer returnLength:length commandBuffer:cBuffer commandLength:cLength];
}

- (void) makeTaskCountCommandBuffer:(unsigned char*) buffer
                      returnLength:(int*)length{
    MemoryStream stream;
    
    stream.writeByte(0x23);         // command
    
    stream.copyTo(buffer);
    *length = stream.length();
}
- (void) makeTaskCountBuffer:(unsigned char*) buffer
               returnLength:(int*)length{
    byte cBuffer[255];
    memset(cBuffer, 0, sizeof(cBuffer));
    int cLength = 0;
    [self makeTaskCountCommandBuffer:cBuffer returnLength:&cLength];
    
    [self makeSendBuffer:buffer returnLength:length commandBuffer:cBuffer commandLength:cLength];
}
- (void) makeTasksCommandBuffer:(unsigned char*) buffer
                       returnLength:(int*)length
                       packetNo:(int)packetNo{
    MemoryStream stream;
    
    stream.writeByte(0x24);         // command
    stream.writeBCDByte(packetNo);  // packetNo
    
    stream.copyTo(buffer);
    *length = stream.length();
}
- (void) makeTasksBuffer:(unsigned char*) buffer
                returnLength:(int*)length
                packetNo:(int)packetNo{
    byte cBuffer[32];
    memset(cBuffer, 0, sizeof(cBuffer));
    int cLength = 0;
    [self makeTasksCommandBuffer:cBuffer returnLength:&cLength packetNo:packetNo];
    
    [self makeSendBuffer:buffer returnLength:length commandBuffer:cBuffer commandLength:cLength];
}
- (void) makeTasksAckCommandBuffer:(unsigned char*) buffer
                      returnLength:(int*)length{
    MemoryStream stream;
    
    stream.writeByte(0x25);         // command
    byte status = 0;
    stream.writeByte(status);       // status
    
    stream.copyTo(buffer);
    *length = stream.length();
}
- (void) makeTasksAckBuffer:(unsigned char*) buffer
               returnLength:(int*)length{
    byte cBuffer[32];
    memset(cBuffer, 0, sizeof(cBuffer));
    int cLength = 0;
    [self makeTasksAckCommandBuffer:cBuffer returnLength:&cLength];
    
    [self makeSendBuffer:buffer returnLength:length commandBuffer:cBuffer commandLength:cLength];
}

- (IBAction)SyncTime:(UIButton *)sender {
    byte buffer[512];
    memset(buffer, 0, sizeof(buffer));
    int length = 0;
    [self makeSyncTimeBuffer:buffer returnLength:&length];
    
    [self sendTxBuffer:buffer sendLength:length];
}
- (IBAction)Download:(UIButton *)sender {
    byte buffer[512];
    memset(buffer, 0, sizeof(buffer));
    int length = 0;
    [self makeDownloadBuffer:buffer returnLength:&length];
    
    [self sendTxBuffer:buffer sendLength:length];
}
- (IBAction)Upload:(UIButton *)sender {
    byte buffer[512];
    memset(buffer, 0, sizeof(buffer));
    int length = 0;
    [self makeTaskCountBuffer:buffer returnLength:&length];
    
    [self sendTxBuffer:buffer sendLength:length];
}
- (IBAction)ClearRxText:(UIButton *)sender {
    _rxLabel.text = @"";
}

- (void) sendTxBuffer:(unsigned char*) buffer
         sendLength:(int)length{
    NSData *data = [[NSData alloc] initWithBytes:buffer length:(uint)length];
    [_shield writeValue:[CBUUID UUIDWithString:BS_SERIAL_SERVICE_UUID]
     characteristicUUID:[CBUUID UUIDWithString:BS_SERIAL_TX_UUID]
                        p:_peripheral
                        data:data];
}

@end
