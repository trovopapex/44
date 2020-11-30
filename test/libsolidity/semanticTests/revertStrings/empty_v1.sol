pragma abicoder v1;
contract C {
	function f() public {
		revert("");
	}
	function g(string calldata msg) public {
		revert(msg);
	}
}
// ====
// EVMVersion: >=byzantium
// compileViaYul: false
// revertStrings: debug
// ----
// f() -> FAILURE, hex"08c379a0", 0x20, 0
// g(string): 0x20, 0, "" -> FAILURE, hex"08c379a0", 0x20, 0
// g(string): 0x20, 0 -> FAILURE, hex"08c379a0", 0x20, 0
