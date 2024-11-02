namespace OW {
    namespace offset {

        // Base

        static constexpr auto Address_viewmatrix_base = 0x33EE770;//SIG:48 03 0D ? ? ? ? 89 44 24 28 48 B8 ? ? ? ? ? ? ? ?
        static constexpr auto Address_viewmatrix_base_test = 0x3ABFE98;//SIG: 48 8B 0D ? ? ? ? 45 33 C0 E8 ? ? ? ? 48 8B F0
        static constexpr auto Address_entity_base = 0x33D9980;//SIG:48 8b 05 ? ? ? ? 48 03 c9 89 54 c8 ? ff 0d ? ? ? ? 89 15 ? ? ? ? c7 87
        static constexpr auto offset_viewmatrix_ptr = 0x7E0; //ok
        constexpr auto offset_viewmatrix_xor_key = 0x77482B5E10C5793Bi64;
        constexpr auto offset_viewmatrix_xor_key2 = 0xE17AD062B3DD5C8Fui64;
        constexpr auto offset_viewmatrix_xor_key3 = 0x34F79E3A5B5276BEi64;
        static constexpr auto changefov = 0x3550BB8;//SIG:F3 0F 11 05 ? ? ? ? 48 85 C9 74 ? 48 8D 55 ? E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 85 C0 74 ? 48 8B 15 ? ? ? ? 48 8D 0D ? ? ? ? FF D0 8B 43

        static constexpr auto SensitivePtr = 0x2064; //good
        static constexpr auto view_angle = 0x11A0;
        static constexpr auto Input = 0x115C;
    }
}

//decryptcomp:48 89 5c 24 ? 48 89 6c 24 ? 56 57 41 56 48 83 ec ? 49 8b f1 49 8b e8
        //HeapManager SIG:84 C0 74 19 48 8B 0D ? ? ? ?(fifth sub)