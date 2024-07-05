// SPDX-License-Identifier: GPL-3.0-only
pragma solidity ^0.8.20;

import { HorseStore } from "../../src/horseStoreV1/HorseStore.sol";
import {IHorseStore} from "../../src/horseStoreV1/IHorseStore.sol";
import {Test,console2} from "forge-std/Test.sol";

abstract contract BaseTestV1 is Test {
    IHorseStore public horsestore;

    function setUp() public virtual {
        horsestore = IHorseStore(address(new HorseStore()));
    } 

    function testReadValue() public {
        uint256 initialValue = horsestore.readNumberOfHorses();
        assertEq(initialValue, 0);
    }

    function testWriteValue(uint256 numberOfHorses) public {
        horsestore.updateHorseNumber(numberOfHorses);
        assertEq(horsestore.readNumberOfHorses(), numberOfHorses);
    }
}