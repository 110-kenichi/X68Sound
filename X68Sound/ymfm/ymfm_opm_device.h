// BSD 3-Clause License
//
// Copyright (c) 2022, Kenichi Ito
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "windows.h"
#include "ymfm_opm.h"

#ifndef YMFM_OPM_DEVICE_H
#define YMFM_OPM_DEVICE_H

#pragma once

namespace ymfm
{
	class ymfm_opm_device : public ymfm::ymfm_interface
	{
	public:
		ymfm_opm_device();

		~ymfm_opm_device();

		// 初期化
		BOOL Init(DWORD c, DWORD r, BOOL f);
		// リセット
		void Reset();
		// レジスタ設定
		void SetReg(DWORD addr, DWORD data);
		// 合成
		void Generate(int* buffer);

	protected:

	private:
		ymfm::ym2151 m_opm;
		//チップのクロック
		unsigned int clock;
		//チップ内部のサンプルレート
		unsigned int real_sample_rate;
		//xm6のサンプルレート
		unsigned int sample_rate;

		//生成したPCMデータを保持するバッファ
		std::vector<ymfm::ym2151::output_data> samples;

		//サンプルレートに合わせてPCMデータを生成
		void generate_core(ymfm::ym2151::output_data* output);
		double generate_counter;
		double generate_step;
		ymfm::ym2151::output_data last_data;

		//フィルター
		int filter;
		int InpInpOpm[2];
		int InpOpm[2];
		int InpInpOpm_prev[2];
		int InpOpm_prev[2];
		int InpInpOpm_prev2[2];
		int InpOpm_prev2[2];
		int OpmHpfInp[2];
		int OpmHpfInp_prev[2];
		int OpmHpfOut[2];;
	};
}

#endif // YMFM_OPM_DEVICE_H
#pragma once
