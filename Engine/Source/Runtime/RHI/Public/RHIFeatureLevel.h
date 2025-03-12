#pragma once

/**
 * RHI의 기능 수준은 신뢰할 수 있는 지원 수준을 나타냅니다.
 * 참고: 기능 수준은 ES3 같은 그래픽 API의 이름을 따랐지만, 기능 수준은 다른 API와 함께 사용할 수 있습니다 (예: ERHIFeatureLevel::ES3.1을 D3D11에서 사용).
 * 그래픽 API가 기능 수준의 모든 기능을 지원하는 한 (예: OpenGL ES3.1에서 ERHIFeatureLevel::SM5는 지원하지 않음).
 */
namespace ERHIFeatureLevel
{
    enum Type : int
    {
        /** OpenGL ES2의 기본 기능으로 정의된 기능 수준. 사용 중단됨 */
        ES2_REMOVED,

        /** OpenGL ES3.1 및 Metal/Vulkan의 기본 기능으로 정의된 기능 수준. */
        ES3_1,

        /**
         * DX10 Shader Model 4의 기능으로 정의된 기능 수준.
         * 이 기능 수준에 대한 지원은 완전히 제거되었습니다.
         */
        SM4_REMOVED,

        /**
         * DX11 Shader Model 5의 기능으로 정의된 기능 수준.
         *   공유 메모리, 그룹 동기화, UAV 쓰기, 정수 원자 조작이 포함된 계산 셰이더
         *   간접 드로잉
         *   UAV 쓰기가 포함된 픽셀 셰이더
         *   큐브맵 배열
         *   읽기 전용 깊이 또는 스텐실 뷰 (예: 깊이 버퍼를 SRV로 읽으면서 깊이 테스트 및 스텐실 쓰기 수행)
         * 테셀레이션은 기능 수준 SM5의 일부로 간주되지 않으며 별도의 기능 플래그를 가집니다.
         */
        SM5,

        /**
         * Shader Model 6.5와 DirectX 12 하드웨어 기능 수준 12_2의 기능으로 정의된 기능 수준.
         *   레이 트레이싱 티어 1.1
         *   메쉬 및 증폭 셰이더
         *   가변 속도 셰이딩
         *   샘플러 피드백
         *   리소스 바인딩 티어 3
         */
        SM6,

        Num
    };
};
