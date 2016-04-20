//
//  ViewController.m
//  speexdemo
//
//  Created by kimziv on 16/4/20.
//  Copyright © 2016年 kimziv. All rights reserved.
//

#import "ViewController.h"
#include "speex/speex.h"
@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  speex_bits_init(NULL);
  // Do any additional setup after loading the view, typically from a nib.
}

- (void)didReceiveMemoryWarning {
  [super didReceiveMemoryWarning];
  // Dispose of any resources that can be recreated.
}

@end
