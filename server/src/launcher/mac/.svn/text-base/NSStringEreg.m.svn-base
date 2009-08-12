 /*
	This file is part of MAMP
 
	MAMP is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published
	by the Free Software Foundation; either version 2, or (at your
	option) any later version.

	MAMP is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with MAMP; see the file Licence.rtf.  If not, write to the
	Free Software Foundation, Inc., 59 Temple Place - Suite 330,
	Boston, MA 02111-1307, USA.
	
	
*/

#include <stdio.h>
#include <stdlib.h>
#include <regex.h>   // Comment out if your system does not have it.
#include <string.h>
#include <ctype.h>

#import "NSStringEreg.h"


@implementation NSString (EregAdditions)

-(BOOL)ereg: (NSString *)pattern regs:(NSMutableArray *)array
{
    regex_t regexp;
    int subidx_count = 0;
    int substring_count = 0;
    int substr_length = 0;
    int status = 0;
    int nsubs  = 0;
    int i = 0;
    int j = 0;
    int k = 0;
	int err_msg_sz = 0;

    regmatch_t *subidx  = NULL;
	char *substr = NULL;
    char  *err_msg      = NULL;
	NSString *regstring = Nil;
	
    
    if(pattern == Nil) {
		NSLog(@"Nil pointer passed as argument to cs_v_regex.");
		return NO;
	}else if([self length] == 0 || [pattern length] == 0 ) {
		//NSLog(@"Empty string passed as argument to [ereg ereg].");
        return NO;
    }

    /* Compile the regular expression pattern. */
    if( ( status = regcomp( &regexp, [pattern UTF8String], REG_EXTENDED ) ) != 0 ){
		/* Get the size of the message.  */
		err_msg_sz = regerror( status, &regexp, NULL, (size_t) 0 );
		/* Allocate the memory, print the message, and then free the memory.  */
		if( ( err_msg = (char *) malloc( err_msg_sz ) ) != NULL ){
			regerror( status, &regexp, err_msg, err_msg_sz );
			NSLog(@"%s", err_msg );
			free( err_msg );
			err_msg = NULL;
		}
        return NO;
    }

    nsubs  = [pattern length];
    
    /* Allocate memory for the array of structs that hold
       the indices of the substring matches. */
    subidx = (regmatch_t*) malloc( nsubs * sizeof(regmatch_t) );
    if( subidx == NULL ){
		NSLog(@"Memory allocation failure for index vector." );
        regfree( &regexp );
        return NO;
		
    }else{ /* Memory was allocated.  */
	
        /* Check for a regex match and save substrings */
        status = regexec( &regexp, [self UTF8String], (size_t) nsubs, subidx, 0 );

        if( status == 0 ) { /* Successful search */
        
			if(array == Nil){
				return YES;
			}
			
			[array removeAllObjects];

		
            /* Count the number of substring matches */
            for( i = 0; i < nsubs; i++ ) {
			
                if( subidx[i].rm_so == -1 || subidx[i].rm_so == -1 ){
                    break;
				}
                subidx_count++;
            }

            /* Make sure there is at least 1 valid set of substring
               indices listed in the array of structs.  */
            if( subidx_count > 0 ){
			
                for( i = 0; i < subidx_count; i++ ){
                    /* Allocate memory for each of the substring matches. */
                    substr_length = subidx[i].rm_eo - subidx[i].rm_so;
                    if( substr_length > 0 ) {
                        substr = (char*) malloc( substr_length + 1 );
                        substr[substr_length] = '\0';
                        if( substr == NULL ) {
							NSLog(@"Memory allocation failure on substring." );
                            free( subidx );
                            regfree( &regexp );
                            return NO;
                        }else{
                            /* Copy the substring into the vector. */
                            k = 0;
							
                            for( j = (int)subidx[i].rm_so; j < (int)subidx[i].rm_eo; j++, k++ ){
                                substr[k] = ([self UTF8String])[j];
							}
							regstring = [[NSString alloc]initWithUTF8String:substr];
							[array addObject:regstring];
							[regstring release];
							free(substr);
                            substring_count++;
                        }
                    } else { /* The indices are invalid. */
                        break;
                    }

                }  /* close  =>  for(i = 0; i < subidx_count; i++) */

            }  /* close  =>  if(subidx_count > 0) */

        }  /* close  =>  if(status == 0) */
        
        free( subidx );

    }  /* close  =>  else  {subidx != NULL}  */

	regfree( &regexp );
	
	return status == 0;
}

-(NSString *)ereg_replace:(NSString *)pattern replaceWith:(NSString *)replace
{
	NSMutableArray *regs = [NSMutableArray array];
	NSMutableString *tmpString = [NSMutableString stringWithString:self];
	NSMutableString *replaceString = [NSMutableString stringWithString:replace];
	int i=0;
	
	if([tmpString ereg:pattern regs:regs]){
		for(i=0; i<[regs count]; i++){
			[replaceString replaceOccurrencesOfString:[NSString stringWithFormat:@"%c",i] 
						withString:[regs objectAtIndex:i] 
						options:NSLiteralSearch 
						range:NSMakeRange(0, [replaceString length])];
		}
	
		while([tmpString ereg:pattern regs:regs]){
			[tmpString replaceOccurrencesOfString:[regs objectAtIndex:0] 
						withString:replaceString 
						options:NSLiteralSearch 
						range:NSMakeRange(0, [tmpString length])];
		}
	}
	
	return [NSString stringWithString:tmpString];
}

-(NSMutableString *)quotemeta
{
	NSMutableString *out = [[NSMutableString alloc] init];
	int i = 0;
	for(i=0; i<[self length]; i++){
		switch([self characterAtIndex:i]){
			case '.':
			case '+':
			case '*':
			case '?':
			case '[':
			case ']':
			case '^':
			case '(':
			case ')':
			case '$':
			case '\\':
				[out appendFormat:@"\\%c",[self characterAtIndex:i]];
				break;
			default:
				[out appendFormat:@"%c",[self characterAtIndex:i]];
		}
	}
	[out autorelease];
	return out;
}
@end