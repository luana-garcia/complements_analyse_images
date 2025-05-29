import numpy as np
from skimage.morphology import skeletonize, remove_small_objects, closing, disk
from skimage.filters import frangi, gaussian, threshold_otsu
from skimage.exposure import equalize_adapthist
from skimage.measure import label
from PIL import Image
from scipy import ndimage as ndi
from skimage.util import img_as_ubyte
from skimage import color, measure
from matplotlib import pyplot as plt

def enhanced_segmentation_v1(img, img_mask):
    # Pré-traitement
    img_eq = equalize_adapthist(img, clip_limit=0.03)
    img_smooth = gaussian(img_eq, sigma=1)
    
    # Filtre de Frangi pour détection des vaisseaux
    img_frangi = frangi(img_smooth, sigmas=range(1, 3, 1), 
                       alpha=0.5, beta=0.5, gamma=15)
    
    # Seuillage adaptatif
    thresh = threshold_otsu(img_frangi)
    img_bin = img_frangi > thresh
    
    # Post-traitement
    img_bin = remove_small_objects(img_bin, min_size=50)
    img_bin = closing(img_bin, disk(1))
    
    return img_bin & img_mask

def enhanced_segmentation_v2(img, img_mask):
    # Pré-traitement
    img_eq = equalize_adapthist(img, clip_limit=0.03)
    img_smooth = gaussian(img_eq, sigma=1)
    
    # Filtre de Frangi pour détection des vaisseaux
    img_frangi_fin = frangi(img_smooth, sigmas=range(1, 2), alpha=0.5, beta=0.5)  # Vaisseaux fins
    img_frangi_epais = frangi(img_smooth, sigmas=range(3, 5), alpha=0.5, beta=0.5) # Vaisseaux larges
    img_frangi_combine = np.maximum(img_frangi_fin, img_frangi_epais)  # Fusion
    
    # Seuillage adaptatif
    thresh = threshold_otsu(img_frangi_combine)
    img_bin = img_frangi_combine > thresh
    
    # Post-traitement
    img_bin = remove_small_objects(img_bin, min_size=50)
    
    labels = label(img_bin)
    regions = measure.regionprops(labels)
    for region in regions:
        if region.area < 100:  # Supprime les régions trop petites
            img_bin[labels == region.label] = 0
    
    return img_bin & img_mask

def evaluate(img_out, img_GT):
    GT_skel = skeletonize(img_GT)
    img_out_skel = skeletonize(img_out)
    
    TP = np.sum(img_GT & img_out)  # Vrais positifs
    FP = np.sum(img_out_skel & ~img_GT)  # Faux positifs
    FN = np.sum(GT_skel & ~img_out)  # Faux négatifs
    
    ACCU = TP / (TP + FP) if (TP + FP) > 0 else 0  # Precision
    RECALL = TP / (TP + FN) if (TP + FN) > 0 else 0  # Rappel
    F1 = 2 * (ACCU * RECALL) / (ACCU + RECALL) if (ACCU + RECALL) > 0 else 0
    
    return ACCU, RECALL, F1, img_out_skel, GT_skel

def segment_image(num_image):
    img_path = './images_IOSTAR/'

    # Chargement de l'image
    img = np.asarray(Image.open(f"{img_path}{dic_images[num_image]['src']}")).astype(np.uint8)
    nrows, ncols = img.shape
    row, col = np.ogrid[:nrows, :ncols]

    # Masque circulaire
    img_mask = (np.ones(img.shape)).astype(np.int32)
    invalid_pixels = ((row - nrows/2)**2 + (col - ncols/2)**2 > (nrows / 2)**2)
    img_mask[invalid_pixels] = 0

    # Segmentation améliorée
    img_out = enhanced_segmentation_v2(img, img_mask)

    # Chargement de la vérité terrain
    img_GT = np.asarray(Image.open(f"{img_path}{dic_images[num_image]['gt']}")).astype(np.int32)

    # Évaluation
    ACCU, RECALL, F1, img_out_skel, GT_skel = evaluate(img_out, img_GT)
    print(f'Precision = {ACCU:.3f}, Recall = {RECALL:.3f}, F1-score = {F1:.3f}')

    # Visualisation
    plt.figure(figsize=(12, 8))
    plt.subplot(231)
    plt.imshow(img, cmap='gray')
    plt.title('Image Originale')
    plt.subplot(232)
    plt.imshow(img_out, cmap='gray')
    plt.title('Segmentation améliorée')
    plt.subplot(233)
    plt.imshow(img_out_skel, cmap='gray')
    plt.title('Squelette segmentation')
    plt.subplot(234)
    plt.imshow(img_GT, cmap='gray')
    plt.title('Vérité Terrain')
    plt.subplot(235)
    plt.imshow(GT_skel, cmap='gray')
    plt.title('Squelette GT')
    plt.subplot(236)
    plt.imshow(np.abs(img_out.astype(int) - img_GT.astype(int)), cmap='jet')
    plt.title('Différence GT/Segmentation')
    plt.tight_layout()
    plt.show()

dic_images = {
    0: {"src": "star01_OSC.jpg", "gt": "GT_01.png"},
    1: {"src": "star02_OSC.jpg", "gt": "GT_02.png"},
    2: {"src": "star03_OSN.jpg", "gt": "GT_03.png"},
    3: {"src": "star08_OSN.jpg", "gt": "GT_08.png"},
    4: {"src": "star21_OSC.jpg", "gt": "GT_21.png"},
    5: {"src": "star26_ODC.jpg", "gt": "GT_26.png"},
    6: {"src": "star28_ODN.jpg", "gt": "GT_28.png"},
    7: {"src": "star32_ODC.jpg", "gt": "GT_32.png"},
    8: {"src": "star37_ODN.jpg", "gt": "GT_37.png"},
    9: {"src": "star48_OSN.jpg", "gt": "GT_48.png"}
}

for i in dic_images:
    segment_image(i)