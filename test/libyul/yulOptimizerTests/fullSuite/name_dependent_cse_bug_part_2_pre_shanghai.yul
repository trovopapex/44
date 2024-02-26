{
    main(0, 0)

    function main(a, b) {
        for {} 1 {}
        {
            if iszero(a) { break }

            let mid := avg(a, a)
            switch a
            case 0 {
                a := mid
            }
            default {
                sstore(0, mid)
            }
        }
    }

    function avg(x, y) -> var {
        // NOTE: Variable names should not affect CSE.
        // This should not be optimized differently than name_dependent_cse_bug_part_1_pre_shanghai.yul.
        // `let mid := var` should be present in both or missing in both.
        let _2 := add(x, y)
        var := add(_2, _2)
    }
}
// ====
// EVMVersion: <shanghai
// ----
// step: fullSuite
//
// {
//     {
//         let _1 := 0
//         let a := 0
//         for { } a { }
//         {
//             let _2 := add(a, a)
//             let var := add(_2, _2)
//             switch a
//             case 0 { a := var }
//             default { sstore(_1, var) }
//         }
//     }
// }
