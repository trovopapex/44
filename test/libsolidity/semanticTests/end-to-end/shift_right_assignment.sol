
		contract C {
			function f(uint a, uint b) public returns (uint) {
				a >>= b;
				return a;
			}
		}
	
// ----
// f(uint256,uint256): 0x4266, 0x0 -> 0x4266
// f(uint256,uint256): 0x4266, 0x8 -> 0x42
// f(uint256,uint256): 0x4266, 0x10 -> 0
// f(uint256,uint256): 0x4266, 0x11 -> 0

