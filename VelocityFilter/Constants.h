//
//  Constants.h
//  VelocityFilter
//
//  Created by Hans on 14/3/16.
//  Copyright © 2016 Hans. All rights reserved.
//

#ifndef Constants_h
#define Constants_h

#define BM_DB_TO_GAIN(db) pow(10.0,db/20.0)
#define BM_GAIN_TO_DB(gain) log10f(gain)*20.0

#endif /* Constants_h */
