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

#include "ymfm_opm.h"
#include "ymfm_opm_device.h"

namespace ymfm
{

	ymfm_opm_device::ymfm_opm_device() :
		m_opm(*this),
		real_sample_rate(0),
		sample_rate(0),
		clock(0),
		generate_counter(0),
		generate_step(0),
		last_data()
	{
	}

	ymfm_opm_device::~ymfm_opm_device()
	{

	}


	BOOL ymfm_opm_device::Init(DWORD c, DWORD r, BOOL f)
	{
		clock = c;
		real_sample_rate = m_opm.sample_rate(c);
		sample_rate = r;

		generate_step = (double)real_sample_rate / (double)sample_rate;

		return TRUE;
	}

	void ymfm_opm_device::Reset()
	{
		m_opm.reset();
		generate_counter = 0;
		samples.clear();

		for (int i = 0; i < 2; i++) {
			InpInpOpm[i] = 0;
			InpOpm[i] = 0;
			InpInpOpm_prev[i] = 0;
			InpOpm_prev[i] = 0;
			InpInpOpm_prev2[i] = 0;
			InpOpm_prev2[i] = 0;
			OpmHpfInp[i] = 0;
			OpmHpfInp_prev[i] = 0;
			OpmHpfOut[i] = 0;
		}
	}

	//サンプルレートに合わせてPCMデータを生成
	//TODO: ymfm サンプルレートが44.1/48KHzの場合のフィルタ処理が必要だが、Mix()のnsampleが短すぎてフィルタが思うように効かない。
	//xm6側で、ある程度の大きなブロックごとにフィルタを掛けるような改造が必要
	void ymfm_opm_device::generate_core(ymfm::ym2151::output_data* output)
	{
		generate_counter += generate_step;
		if (generate_counter >= 1)
		{
			//次のPCMデータ(複数)を取得する

			ymfm::ym2151::output_data output_data;
			output_data.data[0] = 0;
			output_data.data[1] = 0;

			int cnt = (int)(generate_counter + 0.5);
			generate_counter -= cnt;

			for (int i = 0; i < cnt; i++)
			{
				//ymfmに生成を依頼
				ymfm::ym2151::output_data tmp;
				m_opm.generate(&tmp);

				output_data.data[0] += tmp.data[0];
				output_data.data[1] += tmp.data[1];
			}
			//簡単に平均を取る
			output_data.data[0] /= cnt;
			output_data.data[1] /= cnt;

			last_data = output_data;
		}
		*output = last_data;
	}

	void ymfm_opm_device::SetReg(DWORD addr, DWORD data)
	{
		m_opm.write_address(addr);
		m_opm.write_data(data);

		//TODO: ymfm 本来はFMチップのクロックに同期して書き込み＆WAVE生成を並行して行う必要があるがxm6の大改造が必要
		//HACK: ymfm キーオン/オフだけはFMチップのクロックを１つ進める(そうしないとキーオンが抜ける)
		if (addr == 0x8)
		{
			ymfm::ym2151::output_data output;
			generate_core(&output);
			samples.emplace_back(output);
		}
	}

	void ymfm_opm_device::Generate(int* buffer)
	{
		ymfm::ym2151::output_data output;

		if (samples.size() == 0)
		{
			generate_core(&output);
			buffer[0] = output.data[0];
			buffer[1] = output.data[1];
		}
		else {
			buffer[0] = samples[0].data[0];
			buffer[1] = samples[0].data[1];
			samples.erase(samples.begin());
		}
	}
}