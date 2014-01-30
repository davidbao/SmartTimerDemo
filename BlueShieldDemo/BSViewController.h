//
//  BSViewController.h
//  BlueShieldDemo
//
//  Created by yy on 13-4-14.
//  Copyright (c) 2013年 Linlinqi. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BlueShield.h"
#import "BSDefines.h"

@interface BSViewController : UITableViewController

@property (nonatomic, strong) BlueShield *shield;

- (IBAction)reloadClicked:(id)sender;

@end
