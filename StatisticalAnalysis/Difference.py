import numpy as np
from scipy.stats import chi2
import matplotlib.pyplot as plt
from scipy.odr import ODR, Model, RealData

det_idx = 5 # NOTE: Detector indexing start at 0
det_idx+=1 # NOTE: Because the first must be ignore since is the initial state
PARTICLE_NUMBER = 10000
RUN_INDEX = 0

FOUNDAMENTAL_CHARGE = 1.602176634e-19;
LIGHT_SPEED = 299792458.;

MIN_PARTICLE_MASS = (0.5e6 * FOUNDAMENTAL_CHARGE) / (LIGHT_SPEED * LIGHT_SPEED);
MAX_PARTICLE_MASS = (300e6 * FOUNDAMENTAL_CHARGE) / (LIGHT_SPEED * LIGHT_SPEED);

DETECTOR_SPACE_UNCERTAINTY = 1e-6;
DETECTOR_TIME_UNCERTAINTY = 1e-11;

mes_t_dif = []
mes_x_dif = []
mes_y_dif = []
smo_t_dif = []
smo_x_dif = []
smo_y_dif = []

ext_vz_dif = []
ext_vx_dif = []
ext_vy_dif = []
smo_vz_dif = []
smo_vx_dif = []
smo_vy_dif = []

pull_t = []
pull_x = []
pull_y = []

pull_mesmo_t = []

for particle_index in range(PARTICLE_NUMBER):
    if (particle_index % 100 == 0):
        print(particle_index)
    data = np.loadtxt(f"../results/Run {RUN_INDEX} Particle {particle_index}.csv", skiprows=2, unpack=True, delimiter=",", dtype=float)
    z, _, _, _, _, _, _, rea_t, rea_x, rea_y, rea_v, rea_xz, rea_yz, mes_t, mes_x, mes_y, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, smo_t, smo_st, smo_x, smo_sx, smo_y, smo_sy, smo_v, smo_sv, smo_xz, smo_sxz, smo_yz, smo_syz = data
    mes_t_dif.append(mes_t[det_idx] - rea_t[det_idx])
    mes_x_dif.append(mes_x[det_idx] - rea_x[det_idx])
    mes_y_dif.append(mes_y[det_idx] - rea_y[det_idx])
    smo_t_dif.append(smo_t[det_idx] - rea_t[det_idx])
    smo_x_dif.append(smo_x[det_idx] - rea_x[det_idx])
    smo_y_dif.append(smo_y[det_idx] - rea_y[det_idx])

    ext_vz_dif.append((mes_t[det_idx] - mes_t[det_idx-1])/(z[det_idx]-z[det_idx-1]) - rea_v[det_idx])
    ext_vx_dif.append((mes_x[det_idx] - mes_x[det_idx-1])/(z[det_idx]-z[det_idx-1]) - rea_xz[det_idx])
    ext_vy_dif.append((mes_y[det_idx] - mes_y[det_idx-1])/(z[det_idx]-z[det_idx-1]) - rea_yz[det_idx])
    smo_vz_dif.append(smo_v[det_idx] - rea_v[det_idx])
    smo_vx_dif.append(smo_xz[det_idx] - rea_xz[det_idx])
    smo_vy_dif.append(smo_yz[det_idx] - rea_yz[det_idx])

    pull_t.append((smo_t[det_idx] - rea_t[det_idx])/smo_st[det_idx])
    pull_x.append((smo_x[det_idx] - rea_x[det_idx])/smo_sx[det_idx])
    pull_y.append((smo_y[det_idx] - rea_y[det_idx])/smo_sy[det_idx])

    pull_mesmo_t.append((smo_t[det_idx] - mes_t[det_idx])/np.sqrt(DETECTOR_TIME_UNCERTAINTY**2+smo_st[det_idx]**2))
    if np.abs(rea_x).max() < 50.e-6:
        print(particle_index)

mes_t_dif = np.array(mes_t_dif)
mes_x_dif = np.array(mes_x_dif)
mes_y_dif = np.array(mes_y_dif)
smo_t_dif = np.array(smo_t_dif)
smo_x_dif = np.array(smo_x_dif)
smo_y_dif = np.array(smo_y_dif)

ext_vz_dif = np.array(ext_vz_dif)
ext_vx_dif = np.array(ext_vx_dif)
ext_vy_dif = np.array(ext_vy_dif)
smo_vz_dif = np.array(smo_vz_dif)
smo_vx_dif = np.array(smo_vx_dif)
smo_vy_dif = np.array(smo_vy_dif)

pull_t = np.array(pull_t)
pull_x = np.array(pull_x)
pull_y = np.array(pull_y)

pull_mesmo_t = np.array(pull_mesmo_t)

ys = [(mes_t_dif, smo_t_dif), (mes_x_dif, smo_x_dif), (mes_y_dif, smo_y_dif), (ext_vz_dif, smo_vz_dif), (ext_vx_dif, smo_vx_dif), (ext_vy_dif, smo_vy_dif)]
sigmas = [(1e-11,0.7e-11),(1e-6,0.7e-6),(1e-6,0.7e-6),(2.3e-9,0.1e-9),(1.3e-4,0.2e-4),(1.3e-4,0.2e-4)]
names = ["res_t", "res_x", "res_y", "res_vz", "res_xz", "res_yz"]
xlabels = ["Residual on timing", "Residual on position", "Residual on position", "Residual on velocity", "Residual on direction", "Residual on direction"]
for (y,name, sigma, lab) in zip(ys,names, sigmas, xlabels):
    sigma1, sigma2 = sigma
    figure, ax = plt.subplots()
    ax.grid()
    ax.ticklabel_format(style="sci",axis="x", scilimits=(-2,3), useMathText=True)
    measured, smoothed= y
    values, bin_edges, _ = ax.hist(measured, label="measured", bins=50)
    bin_size = bin_edges[1] - bin_edges[0]
    new_bin_number = int(np.ceil((smoothed.max() - smoothed.min())/bin_size))
    values2, bin_edges2, _ = ax.hist(smoothed, label="smoothed", alpha=0.8, bins=new_bin_number)
    # values2, bin_edges2, _ = ax.hist(smoothed, label="smoothed", alpha=0.8, bins=50)
    ax.set_xlabel(lab, fontsize=12)
    ax.set_ylabel("Occurrences", fontsize=12)
    ax.legend(fontsize=12)

    # xs = (bin_edges[:-1] + bin_edges[1:])*0.5 
    # mask = values > 5
    # xs = xs[mask]
    # ys = values[mask]
    # sxs = np.zeros_like(xs)
    # sys = np.sqrt(ys)

    # dati = RealData(xs, ys, sy=sys)
    # modello = Model(lambda pars, x: pars[0]/(pars[2]*np.sqrt(2*np.pi)) * np.exp(-0.5*((x-pars[1])**2)/(pars[2]**2)))
    # par_init = np.array([5e-7, 0, sigma1])
    # risultato = ODR(dati, modello, par_init).run()
    #
    # chi_2, [norm,mu,sigma], [snorm, smu, ssigma] = risultato.sum_square, risultato.beta, risultato.sd_beta
    # ndof = ys.size - 3
    # pval = 1-chi2.cdf(chi_2, ndof)
    # NOTE: Se fai i fit ricordati di cambiare il binning e rimetterlo a 50
    # print(f"{name} misurato")
    # print(f"A={norm}±{snorm}    mu={mu}±{smu}    sigma={sigma}±{ssigma}")
    # print(f"chi2={chi_2},   ndof={ndof},   pvalue={pval}")
    # x = np.linspace(-3*sigma,3*sigma,1000)
    # y = norm/(sigma*np.sqrt(2*np.pi)) * np.exp(-0.5*((x-mu)**2)/(sigma**2))
    # ax.plot(x,y)

    # xs = (bin_edges2[:-1] + bin_edges2[1:])*0.5 
    # mask = values2 > 5
    # xs = xs[mask]
    # ys = values2[mask]
    # sxs = np.zeros_like(xs)
    # sys = np.sqrt(ys)

    # dati = RealData(xs, ys, sy=sys)
    # modello = Model(lambda pars, x: pars[0]/(pars[2]*np.sqrt(2*np.pi)) * np.exp(-0.5*((x-pars[1])**2)/(pars[2]**2)))
    # par_init = np.array([3200, 0, sigma2])
    # risultato = ODR(dati, modello, par_init).run()

    # chi_2, [norm,mu,sigma], [snorm, smu, ssigma] = risultato.sum_square, risultato.beta, risultato.sd_beta
    # ndof = ys.size - 3
    # pval = 1-chi2.cdf(chi_2, ndof)
    # print(f"{name} smoothed")
    # print(f"A={norm}±{snorm}    mu={mu}±{smu}    sigma={sigma}±{ssigma}")
    # print(f"chi2={chi_2},   ndof={ndof},   pvalue={pval}\n\n")
    # x = np.linspace(-3*sigma,3*sigma,1000)
    # y = norm/(sigma*np.sqrt(2*np.pi)) * np.exp(-0.5*((x-mu)**2)/(sigma**2))
    # ax.plot(x,y)

    figure.savefig(f"../figures/StatisticalAnalysis/{name}.pdf", bbox_inches="tight")
    plt.close(figure)


ys = [pull_t, pull_x, pull_y, pull_mesmo_t]
names = ["pull_t", "pull_x", "pull_y", "pull_mesmo"]
xlabels = ["Pull on timing", "Pull on position", "Pull on position", "Pull smooth-measured timing"]
for (y,name, lab) in zip(ys,names, xlabels):
    figure, ax = plt.subplots()
    x = np.linspace(pull_x.min(), pull_x.max(), 1000)
    ax.grid()
    values, bin_edges, _ = ax.hist(y, label=r"$\frac{x_t - x_r}{\sigma_t}$", bins=48)
    area = ((bin_edges[1:]-bin_edges[:-1])*values).sum()

    ax.plot(x,  area/np.sqrt(2*np.pi) * np.exp(-0.5*x**2), label=r"$y=\frac{A}{\sqrt{2\pi}}e^{-\frac{x^2}{2}}$" )
    ax.set_xlabel(lab, fontsize=12)
    ax.set_ylabel("Occurrences", fontsize=12)
    ax.legend(fontsize=12)
    figure.savefig(f"../figures/StatisticalAnalysis/{name}.pdf", bbox_inches="tight")
    plt.close(figure)

    # xs = (bin_edges[1:]+bin_edges[:-1])/2.
    # mask = values > 5
    # xs = xs[mask]
    # ys = values[mask]
    # chi2value = (((ys - area/np.sqrt(2*np.pi) * np.exp(-0.5*xs**2))/np.sqrt(ys))**2).sum()
    #
    # ndof = ys.size - 1
    # pval = 1-chi2.cdf(chi2value, ndof)
    # print(f"{name}:    chi2={chi2value},   ndof={ndof},   pvalue={pval}")

