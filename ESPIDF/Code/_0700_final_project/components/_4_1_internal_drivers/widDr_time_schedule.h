// Function
// - driver runtime and check time schedule container
//      - browse and check timer containter
//      - clear set time flag when timer time out
// - API handle config schedule container
//      - regiter a node timer and get pointer to timer node
//      - order time and set up timer
//      - check timeout
// - Note: not supply a API delete timer, container only get
//      bigger when register new timer
// - Advantage: Prevent heap fragmentation when destroys timer
// - Disadvantage: 
//      The container can get big and slow down because it uses linked list traversal.
//      When there is a new update from the system, you have to reset to reload from the beginning.
//
// - Note: To simplify the system I will not provide api to modify the file system
//      So after reset all timer state will clear
