//? 0x6080604052348015600e575f80fd5b5060a58061001b5f395ff3fe6080604052348015600e575f80fd5b50600436106030575f3560e01c8063cdfead2e146034578063e026c017146045575b5f80fd5b6043603f3660046059565b5f55565b005b5f5460405190815260200160405180910390f35b5f602082840312156068575f80fd5b503591905056fea2646970667358221220b2d4551faecc01323fad45407dc85b5b48ef341912bad2715dd5a65a823928c264736f6c63430008170033

// 3 sections:
// 1. Contract Creation
// 2. Runtime
// 3. Metadata

//* 1. Contract creation - Free memory pointer
PUSH1 0x80                      // [0x80]
PUSH1 0x40                      // [0x80, 0x40]
MSTORE                          // []       // Memory 0x40 -> 0x80 

//! If someone sent value with this call, jump to the 0X0E PC/JumpDest
CALLVALUE                       // [msg.value]
DUP1                            // [msg.value, msg.value]
ISZERO                          // [msg.value == 0, msg.value]
PUSH1 0x0e                      // [0x0E, msg.value == 0, msg.value]
JUMPI                           // [msg.sender]
PUSH0                           // [0x00,msg.sender]
DUP1                            // [0x00,0x00,msg.sender]
REVERT                          // [msg.sender]

//! Jump dest if msg.value == 0
//! Sticks the runtime code on chain
JUMPDEST                        // [msg.sender]
POP                             // []
PUSH1 0xa5                      // [0xa5]
DUP1                            // [0xa5,0xa5]
PUSH2 0x001b                    // [0x001b,0xa5,0xa5]
PUSH0                           // [0x00,0x001b,0xa5,0xa5]
CODECOPY                        // [0xa5]       Memory: [runtime code]
PUSH0                           // [0x00, 0xa5]
RETURN                          // []
INVALID                         // []

//* 2. Runtime Code - Entry points of all calls 
PUSH1 0x80                      
PUSH1 0x40
MSTORE                          // Memory : [0x40: 0x80]

//! Checking for msg.value, and if given, reverting
CALLVALUE                       // [msg.value]
DUP1                            // [msg.value,msg.value]
ISZERO                          // [msg.value==0,msg.value]
PUSH1 0x0e                      // [0x0e, msg.value==0,msg.value]
JUMPI                           // [msg.value]
PUSH0                           // [0x00, msg.value]
DUP1                            // [0x00 , 0x00 , msg.value]
REVERT                          // [msg.value]

//! If msg.value == 0, jump here
//! This is checking to see if there is enough calldata for a function selector
JUMPDEST                        // [msg.value]
POP                             // []
PUSH1 0x04                      // [0x04]
CALLDATASIZE                    // [calldatasize , 0x04]
LT                              // [calldatasize < 0x04]
PUSH1 0x30                      // [0x30 , calldatasize < 0x04]
JUMPI                           // []
//! if calldata_size < 0x40 —> calldata_jump

//* Function dispatching in solidity
PUSH0                         // [0]
CALLDATALOAD                  // [32 bytes of calldata]
PUSH1 0xe0                    // [0xe0, 32bytes of calldata]
SHR                           // [calldata[0:4]]    // function_selector

//* Function dispatching for setNumberOfHorses
DUP1                          // [function_selector,function_selector]
PUSH4 0xcdfead2e              // [0xcdfead2e,function_selector,function_selector]
EQ                            // [function_selector == 0xcdfead2e,function_selector]
PUSH1 0x34                    // [0x34,function_selector==0xcdfead2e,func_selector]
JUMPI                         // [func_selector]
//! if func_selector == Oxcdfead2e —> set_number_of_horses (calldata_jump)


//* Function dispatching for readNumber0fHorses
DUP1                          // [func_selector,func_selector]
PUSH4 0xe026c017              // [0xe026c017,func_selector,func_selector]
EQ                            // [func_selector==0xe026c017,func_selector]
PUSH1 0x45                    // [0x45,func_selector==0xe026c017,func_selector]
JUMPI                         // []
//! if func_selector == Oxcdfead2e —> read_number_of_horses


//! calldata_jump
//! Revert JUMPDEST
JUMPDEST                        // []              
PUSH0                           // [0]
DUP1                            // [0,0]
REVERT                          // []

//! updateHorseNumber JUMPDEST 1
JUMPDEST                   // [func_selector]                            
PUSH1 0x43                 // [0x43 ,func_selector]      
PUSH1 0x3f                 // [0x3f,0x43,func_selector]      
CALLDATASIZE               // [calldata_size,0x3f,0x43,func_selector]        
PUSH1 0x04                 // [0x04,calldata_size,0x3f,0x43,func_selector]   
PUSH1 0x59                 // [0x59,0x04,calldata_size,0x3f,0x43,func_selector]           
JUMP                       // [0x04,calldata_size,0x3f,0x43,func_selector]

//! JUMPDEST 4 
//!  We can finally run an sstore to save our value to storage:
// 1. Function dispatch
// 2. Checked for msg.value
// 3. Checked that calldata is long enough
// 4. Received the number to use from the calldata

JUMPDEST                   // [calldata(of numberToUpdate),0x43,func_selector]        
PUSH0                      // [0, calldata(of numberToUpdate),0x43,func_selector]     
SSTORE                     // [0x43,func_selector]       
JUMP                       // [func_selector]

//! JUMPDEST 5
JUMPDEST                   //  [func_selector]      
STOP                       // [func_selector]    


//! readNumberOfHorses JUMPDEST 1
JUMPDEST                  // [func_selector]          
PUSH0                     // [0, func_selector]       
SLOAD                     // [numHorses, func_selector]       
PUSH1 0x40                // [0x40 , numHorses ,func_selector ]            
MLOAD                // [0x80] // Memory : [0x40: 0x80] (free memory pointer) Loading onto the stack     
SWAP1                     // [numHorses,0x80,func_selector]       
DUP2                      // [0x80,numHorses,0x80,func_selector]      
MSTORE                    // [0x80,func_selector]   // Memory -> 0x80: numHorses
PUSH1 0x20                // [0x20, 0x80 , func_selector]            
ADD                       // [0xa0, func_selector]     
PUSH1 0x40                // [0x40 ,0xa0, func_selector]            
MLOAD                     // [0x80,0xa0, func_selector]       
DUP1                      // [0x80,0x80 ,0xa0, func_selector]     
SWAP2                     // [0xae, 0x80,0x80,func_selector]       
SUB                       // [0x20, 0x80 , func_selector]    
SWAP1                     // [0x80,0x20 , func_selector]       
// Return a value of size 32 bytes, that is located at position 0x80 in memory

RETURN                    // [func_selector]  


//! updateHorseNumber JUMPDEST 2
//! Check to see if there is a value to update the horse number to
//! 4 bytes for function selector, 32 bytes for horse number

JUMPDEST                        // [0x04,calldata_size,0x3f,0x43,func_selector]
PUSH0                           // [0,0x04,calldata_size,0x3f,0x43,func_selector] 
PUSH1 0x20                      // [0x20,0,0x04,calldata_size,0x3f,0x43,func_selector]      
DUP3                            // [0x04,0x20,0,0x04,calldata_size,0x3f,0x43,func_selector]
DUP5                            // [calldata_size,0x04,0x20,0,0x04,calldata_size,0x3f,0x43,func_selector]
SUB                             // [calldata_size-0x04,0x20,0,0x04,calldata_size,0x3f,0x43,func_selector]

//! is there more calldata than just the function selector?
SLT                    // [calldata_size-0x04 > 0x20,0,0x04,calldata_size,0x3f,0x43,func_selector]
ISZERO                 // [more_calldata_than_selector,0,0x04,calldata_size,0x3f,0x43,func_selector]
PUSH1 0x68             //[0x68,more_calldata_than_selector,0,0x04,calldata_size,0x3f,0x43,func_selector]               
JUMPI                  // [0,0x04,calldata_size,0x3f,0x43,func_selector]

//! We are going to jump to JUMPDEST 3 if there is more calldata than: function_selector + 0x20(32 bytes)

PUSH0                     // [0,0,0x04,calldata_size,0x3f,0x43,func_selector]
DUP1                      // [0,0,0,0x04,calldata_size,0x3f,0x43,func_selector]
REVERT                    // [0,0x04,calldata_size,0x3f,0x43,func_selector]

//! updateHorseNumber JUMPDEST 3
JUMPDEST                  // [0,0x04,calldata_size,0x3f,0x43,func_selector]
POP                       // [0x04,calldata_size,0x3f,0x43,func_selector]     

// Ignore the function selector, and just grab the data
// 0xcdfead2e       0000000000000000000000000000000000000000000000000000000000000007
CALLDATALOAD          // [calldata(of numberToUpdate),calldata_size,0x3f,0x43,func_selector]            
SWAP2                 // [0x3f,calldata_size,calldata(of numberToUpdate),0x43,func_selector]  
SWAP1                 // [calldata_size,0x3f,calldata(of numberToUpdate),0x43,func_selector]         
POP                   // [0x3f,calldata(of numberToUpdate),0x43,func_selector]         
JUMP                  
//! jump to JUMPDEST 4


//* 3. Metadata 
INVALID                            
LOG2                             
PUSH5 0x6970667358                            
INVALID                            
SLT                            
KECCAK256                            
INVALID                            
INVALID                            
SSTORE                            
INVALID                            
INVALID                            
INVALID                            
ADD                            
ORIGIN                            
EXTCODEHASH                            
INVALID                            
GASLIMIT                            
BLOCKHASH                            
PUSH30 0xc85b5b48ef341912bad2715dd5a65a823928c264736f6c63430008170033