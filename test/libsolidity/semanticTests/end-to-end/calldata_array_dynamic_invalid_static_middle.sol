
		pragma experimental ABIEncoderV2;
		contract C {
			function f(uint256[][1][] calldata a) external returns (uint) {
				return 42;
			}
			function g(uint256[][1][] calldata a) external returns (uint) {
				a[0];
				return 42;
			}
			function h(uint256[][1][] calldata a) external returns (uint) {
				a[0][0];
				return 42;
			}
		}
	
// ----
// f(uint256[][1][]): 0x20, 0x0 -> 42 # valid access stub #
// f(uint256[][1][]): 0x20, 0x1 ->  # invalid on argument decoding #
// f(uint256[][1][]): 0x20, 0x1, 0x20 -> 42 # invalid on outer access #
// g(uint256[][1][]): 0x20, 0x1, 0x20 -> 
// f(uint256[][1][]): 0x20, 0x1, 0x20, 0x20 -> 42 # invalid on inner access #
// g(uint256[][1][]): 0x20, 0x1, 0x20, 0x20 -> 42
// h(uint256[][1][]): 0x20, 0x1, 0x20, 0x20 -> 
// f(uint256[][1][]): 0x20, 0x1, 0x20, 0x20, 0x1 -> 42
// g(uint256[][1][]): 0x20, 0x1, 0x20, 0x20, 0x1 -> 42
// h(uint256[][1][]): 0x20, 0x1, 0x20, 0x20, 0x1 -> 

