function code = sample(subModeType,ca_code,nsamples,chipRate,samplFreq,sampPhase)
  
				% SAMPLING FUNCTION
  
  if strcmp(subModeType, "BPSK"),   
  	len = length(ca_code);    
		idxs = floor((0:nsamples-1) .* (chipRate/samplFreq) + sampPhase);
		idxs = 1+mod(idxs, len);
  	code = ca_code(1, idxs);    

  else
    disp("****************************************");
    disp("Selected subcarrier modulation not found");
    disp("****************************************");
    code = [];
  
  end
  
  
  
