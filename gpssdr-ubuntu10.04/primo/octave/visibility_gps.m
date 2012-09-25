function visibility_gps(fileName, fs, fif)

  gpsIdSatVec = 1:32;
  gpsBaseId = 0;

  searchfreqspan = 6000;

  gps_cr = 1.023e6;        %% CHIP RATE
  gps_cf = 1575.42e6;      %% SAMPL FREQUENCY
  gps_codetimelen = 0.001;
  gps_coh = 1;             %% coherent code integration
  gps_noncoh = 8;
  gps_cohtime = gps_coh*gps_codetimelen;
  gps_acqsamples = round(gps_cohtime * gps_noncoh * fs);
  gps_searchfreqvec = -searchfreqspan:(1.0/gps_cohtime):searchfreqspan;
  gps_numsearchfreq = length(gps_searchfreqvec);


  %% OPEN THE FILE
  fid=fopen(fileName, "rb");
  if (fid == -1),
    disp(sprintf("File %s does not exist", fileName));
    return;
  end

  signal = fread(fid, [1 round((0.007)*fs)], "int8");
  figure(1);
  easyspec(signal, fs);
  grid on;
  drawnow;

  figure(2);
  hist(signal, [-7:+2:+7]); grid on;
  drawnow;

  %% SEEK THE FILE POSITION
  fseek(fid, round(fs*0.0), SEEK_SET);

PROCESS_GPS=1;

  if PROCESS_GPS,
    %% PROCESS GPS DATA
    tic;
    signal = fread(fid, [1 gps_acqsamples], "int8");

    dwell_length = floor(gps_cohtime * fs);
    fsigint = zeros(gps_noncoh, dwell_length);
    gps_maxcorrs = zeros(1, 32);
    for i=1:gps_noncoh,
      idx = round(gps_cohtime * (i-1) * fs);

      fsigint(i,:) = fft( signal(1, (idx+1):idx+dwell_length) );
    end

    for svId=gpsIdSatVec,
      disp(sprintf("Searching GPS sv %d", svId + gpsBaseId));
      fflush(1);
      %% Select the right satellite
      code = codegen_GPS(svId + gpsBaseId);
      %% Generate PRN code
      codeint = sample("BPSK", code, dwell_length, gps_cr, fs, 0);
      %% Generate a sampled replica of the code
      fcodeint = conj(fft( codeint ));
      %% Generate the FFT coniugate
      maxcorr = zeros(1, dwell_length);
      maxCorrValue = 0; maxCorrIdx = 0;
      maxfreq = 0;
      for freqIdx=1:gps_numsearchfreq,
        %% Start the frequency search
        rotIdx = round( (fif+gps_searchfreqvec(freqIdx)) * gps_cohtime );

        corr    = zeros(1, dwell_length);
        sumcorr = zeros(1, dwell_length);
        for i=1:gps_noncoh,
          %% Integrate non-coherently over time
          rfsigint = circshift( fsigint(i,:) , [1 -rotIdx] );

          corr = abs(ifft(rfsigint .* fcodeint));
          sumcorr = sumcorr + corr;
        end
        if (max(sumcorr) > max(maxcorr)),
          maxcorr = sumcorr;
          maxfreq = gps_searchfreqvec(freqIdx);
        end
      end

      [maxCorrValue, maxCorrIdx] = max(maxcorr);
      gps_maxcorrs(1, svId) = maxCorrValue;
      if (maxCorrValue > 3*mean(maxcorr)),
        printf(" -> found: Doppler %+7.1f CodeShift: %6d xcorr: %.1f\n", ...
               maxfreq, maxCorrIdx, maxCorrValue);
      end

      figure(10);
      hold off; plot(maxcorr(1:fs*gps_codetimelen), "*-b"); grid on;
      title(sprintf("PRN ID %02d", svId+gpsBaseId));
      drawnow;

    end
    figure(11);
    [xtmp, ytmp] = bar(1:32, gps_maxcorrs);
    plot(xtmp, ytmp, "b"); axis([0 33]); grid on; drawnow;
    title("GPS OS search results.");
    gps_elapsed_time = toc;
    disp(sprintf("Elapsed time: %5.1f sec", gps_elapsed_time));
  end %% GPS PROCESSING
