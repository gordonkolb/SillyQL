
//
//  Created by gordie2 on 11/3/20.
//  Copyright © 2020 Gordon Kolb. All rights reserved.
//


#include <string>
#include <iostream>
#include <stdio.h>
#include "xcode_redirect.hpp"
#include "Driver.hpp"

int main(int argc, char * argv[]) {
    std::ios_base::sync_with_stdio(false);
    xcode_redirect(argc, argv);
    cin >> boolalpha;
    cout << boolalpha;
    
    driver test;
    
    test.getOptions(argc,argv);
    test.read_from_user();

}
