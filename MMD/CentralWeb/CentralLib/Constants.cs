using System;

namespace MMD.Central.CentralLib
{
	public enum StagingStatus
	{
		Unchanged = 0,
		New = 1,
		Changed = 2,
		Deleted = 3
	}

	
	/// <summary>
	/// Application error codes with associated idMessage in tblCSW_Message
	/// </summary>
	public enum ErrMsg
	{
		Unknown = 999,
		Min = 1000,
		CommandNotFound = 1001,
		UserNotAuthenticated = 1002,
		GroupTypeNotFound = 2001,
		RecListItemInsertItemNotChecked = 2002,
		BannerInvalidMIMEType = 2003,
		FrameInvalidMIMEType = 2004,
		IdleLoopElementInsertNotChecked = 2005,
		ImageInvalidMIMEType = 2006,
		ScreenMLInvalidMIMEType = 2007,
		VideoInvalidMIMEType = 2008,
		NoFileSpecified = 2009,
		RecListAttrIsRequired = 2010,
		RecListItemAttrIsRequired = 2011,
		InvalidItemId = 2020,
		FileAlreadyExists = 2021,


		Max
	}

	/// <summary>
	/// Application status codes with associated idMessage in tblCSW_Message
	/// </summary>
	public enum StatusMsg 
	{
		Unknown = 4999,
		Min = 5000,
		GroupTypeAdded = 5001,
		GroupTypeDeleted = 5002,
		GroupTypeUpdated = 5003,
		GroupAdded = 5004,
		GroupDeleted = 5005,
		GroupUpdated = 5006,
		RecListItemInserted = 5007,
		RecListItemAdded = 5008,
		RecListItemDeleted = 5009,
		RecListItemAlreadyExists = 5010, //DB
		RecListAdded = 5011,
		RecListAlreadyExists = 5012, //DB
		RecListReverted = 5013,
		RecListSubmit = 5014,
		GenreNotInMediaType = 5015, //DB
		RecListDeleted = 5016,
		RecListSubmitAll = 5017, 
		BannerAdded = 5018, //DB
		ScheduleAdded = 5019,
		ScheduleUpdated = 5020,
		ScheduleDeleted = 5021,
		BannerUpdated = 5022, 
		BannerDeleted = 5023,
		BannerReverted = 5024,
		BannerSubmit = 5025,
		BannerSubmitType = 5026,
		IdleLoopAdded = 5027,
		IdleLoopUpdated = 5028,
		IdleLoopDeleted = 5029,
		IdleLoopReverted = 5030,
		IdleLoopSubmit = 5031,
		IdleLoopSubmitAll = 5032,
		IdleLoopElementAdded = 5033,
		IdleLoopElementUpdated = 5034,
		IdleLoopElementDeleted = 5035,
		IdleLoopElementInserted = 5036,
		StoreConceptNotFound = 5037,
		RecListTypeAdded = 5038,
		RecListTypeAttributeAdded = 5039,
		RecListTypeItemAttributeAdded = 5040,
		RecListTypeAttributeDeleted = 5041,
		RecListTypeItemAttributeDeleted = 5042,
		RecListTypeDeleted = 5043,
		RecListTypeSubmit = 5044,
		RecListTypeSubmitAll = 5045,
		RecListTypeReverted = 5046,
		RecListEdited = 5047,
		RecListItemEdited = 5048,
		RecListItemMovedUp = 5049,
		RecListItemMovedDown = 5050,
		RecListBaseTypeStoreConceptComboAlreadyExists = 5051, //2022
		RecListBaseTypeStoreConceptProductTypeComboAlreadyExists = 5052, //2023
		EventScheduleEdited = 5053,
		EventScheduleRunNow = 5054,

		AttractLoopItemAdded = 6001,
		AttractLoopItemUpdated = 6002,
		AttractLoopItemRemoved = 6003,
		AttractLoopItemMovedUp = 6004,
		AttractLoopItemMovedDown = 6005,
		FeatureRequestAdded = 6006,
		FeatureRequestUpdated = 6007,
		FeatureRequestRemoved = 6008,
		FileAdded = 6009,
		FileUpdated = 6010,
		FileRemoved = 6011,
		StoreUpdated = 6012,
		DeviceUpdated = 6013,
		Max
	}

}
