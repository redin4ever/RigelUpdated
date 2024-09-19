namespace OW {
    namespace offset {
        /*------------------------------------------------------*/

        static constexpr auto Address_viewmatrix_base = 0x3872680;
        static constexpr auto Address_viewmatrix_base_test = 0x3F3F668;
        static constexpr auto Address_entity_base = 0x385DAD0;
        static constexpr auto offset_viewmatrix_ptr = 0x7E0;
        static constexpr auto offset_viewmatrix_xor_key = 0x1B052A80E8A8B205;
        /*------------------------------------------------------*/
        // sig : E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 48 8B 4D ? 33 D2
        static constexpr auto HeapManager = 0x392B4C0;
        static constexpr auto HeapManager_Var = 0x39101CD;
        static constexpr auto HeapManager_Key = 0x81101AAE3CEDC7EF;
        static constexpr auto HeapManager_Pointer = 0x160;
        static constexpr auto changefov = 0x39D7308;  //0x392E1D8
        /*------------------------------------------------------*/
        static constexpr auto SensitivePtr = 0x2064;
        /*------------------------------------------------------*/
        static constexpr auto VisFN = 0x7B3512; //Change 0x7AED32
        static constexpr auto VisRead = 0x3910540; //Change  0x3867900
        static constexpr auto Vis_Key = 0x01FCB6B3AF52B847; //Change 0x6B6DE88A4C694441 / 0x20690E86123DFCF // 0x0DC41E1E4AE4C76A
        /*------------------------------------------------------*/
        static constexpr auto OutlineFN = 0x91EEF2; //Change   0x7B7EB2     
        static constexpr auto OutlineRead = 0x3910540; //Change  0x3867900
        static constexpr auto OutLine_Key = 0x59365E3D1BE91B7A; //Change  0x4DBBA6008B2945A9 / 
        /*------------------------------------------------------*/
        /*------------------------------------------------------*/
        static constexpr auto g_player_controller = 0x11D0; //Change 

    }
}
