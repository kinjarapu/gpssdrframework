% Read samples file from Kai Borre, Akos book and downconvert it so that
% it can be used in gps-sdr v 13.10.2010
%
% change settings.total_msec accordingly
%
% Toolboxes required for this script to run: Signal, Communications

clear; close all;
format ('compact');
format ('long', 'g');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

settings.codeFreq			= 1.023e6;
settings.codeLength			= 1023;
settings.code_duration		= settings.codeFreq / settings.codeLength;

settings.in_file			= './GPSdata-DiscreteComponents-fs38_192-if9_55.bin';
settings.in_if				= 9.548e6;
settings.in_fs				= 38.192e6;
settings.in_dataType		= 'int8';
settings.in_sampsPerCode 	= round(settings.in_fs / settings.code_duration);
settings.in_ts 				= 1 / settings.in_fs;

settings.out_file			= './downConvDownSamp.bin';
settings.out_if				= -38.4e3;
settings.out_fs				= 2.048e6;
settings.out_dataType		= 'int16';
settings.out_sampsPerCode	= round(settings.out_fs / settings.code_duration);
settings.carrFreq			= settings.in_if - settings.out_if;

settings.msec_perread		= 1e2;
settings.total_msec			= 1e3;
settings.nr_reads			= settings.total_msec / settings.msec_perread;
settings.quantBits			= 8;
settings.MaxLevel			= 2.^(settings.quantBits-1) - 1;
settings.ind_I				= 1:2:2*settings.msec_perread*settings.out_sampsPerCode-1;
settings.ind_Q				= 2:2:2*settings.msec_perread*settings.out_sampsPerCode;

settings.filtertaps			= [ 0.00502207425866958, ...
								0.0206619492811362, ...
								0.051567481760408, ...
								0.0949753388061809, ...
								0.138949676529862, ...
								0.1669316112129, ...
								0.1669316112129, ...
								0.138949676529862, ...
								0.0949753388061809, ...
								0.051567481760408, ...
								0.0206619492811362, ...
								0.00502207425866958];

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

[fid_in, message] = fopen(settings.in_file, 'rb');
if (fid_in <= 0)
	sprintf('File not found, aborting\n');
	exit
end
fseek(fid_in, 0, 'bof');
fid_out = fopen(settings.out_file, 'wb');

D = zeros(1, 2*settings.msec_perread*settings.out_sampsPerCode);

for isec = 1:settings.nr_reads,
	sprintf('isec=%i\n',isec);
	data = fread(fid_in, settings.msec_perread*settings.in_sampsPerCode, settings.in_dataType)';
	if (length(data) ~= settings.msec_perread*settings.in_sampsPerCode)
		sprintf('Could not read enough samples; aborting\n');
		exit
	end
	t = 0:settings.in_ts:(settings.msec_perread*settings.in_sampsPerCode-1)*settings.in_ts;
	compl_exp = exp(j*2*pi*settings.carrFreq*t);
	down_conv = data.*compl_exp;
	post_lpf = filter(settings.filtertaps, 1, down_conv);
	resampled = resample(post_lpf, settings.out_fs, settings.in_fs);
	D(settings.ind_I) = real(resampled);
	D(settings.ind_Q) = imag(resampled);
	Level_Norm = 3*sqrt(var(D));
	Partition = (((-settings.MaxLevel:1:settings.MaxLevel-1)+0.5)/settings.MaxLevel)*Level_Norm;
	Squant = quantiz(D,Partition) - 127;
	fwrite(fid_out, Squant, 'int16');
end

fclose(fid_out);


