//
//  BSViewController.m
//  BlueShieldDemo
//
//  Created by yy on 13-4-14.
//  Copyright (c) 2013年 Linlinqi. All rights reserved.
//

#import "BSViewController.h"
#import "MBProgressHUD.h"
#import "BSShieldViewController.h"

@interface BSViewController ()

@end

@implementation BSViewController

- (void)viewDidLoad
{
    [super viewDidLoad];

    _shield = [[BlueShield alloc] init];
    [_shield controlSetup];
    
    [_shield didPowerOnBlock:^(id response, NSError *error) {
        [self reloadClicked:nil];
    }];

}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    if ([segue.identifier isEqualToString:@"shieldSegue"]) {
        BSShieldViewController *shieldController = (BSShieldViewController *)segue.destinationViewController;
        shieldController.peripheral = sender;
        shieldController.shield = _shield;
    }
    
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [_shield.peripherals count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"shieldCell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier forIndexPath:indexPath];
    
    // Configure the cell...
    CBPeripheral *p = [_shield.peripherals objectAtIndex:indexPath.row];
    cell.textLabel.text = p.name;
    
    return cell;
}

#pragma mark - Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Navigation logic may go here. Create and push another view controller.
    CBPeripheral *p = [_shield.peripherals objectAtIndex:indexPath.row];
    [self performSegueWithIdentifier:@"shieldSegue" sender:p];
}

#pragma mark - IBAction

- (IBAction)reloadClicked:(id)sender {
    double timeout = 3;
    [MBProgressHUD showHUDAddedTo:self.tableView animated:YES];
    [_shield findBLEPeripherals:timeout];
    dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(timeout * NSEC_PER_SEC));
    dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
        [self.tableView reloadData];
        [MBProgressHUD hideAllHUDsForView:self.tableView animated:YES];
    });
}

@end
