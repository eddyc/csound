//
//  FileIO.hpp
//  Signals
//
//  Created by Edward Costello on 14/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#ifndef FileIO_hpp
#define FileIO_hpp

#include <functional>
#include <string>

using namespace std;
namespace Signals {
    
    class FileIO {
        
        
    public:
        
        static void openDataset(string, string,
                                function<tuple<double *, size_t, size_t>(size_t, size_t)> allocator,
                                function<void(double *, size_t, size_t)>);
    };
}
#endif /* FileIO_hpp */
